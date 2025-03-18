// BC_CryptoUtils.cpp
#include "BC_CryptoUtils.h"
#include "BC_Utilities.h"

// Системные библиотеки (только для реализации)
#include <vector>
#include <iomanip>
#include <sstream>

// OpenSSL компоненты
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/err.h>

// Константы преобразования
const size_t HEX_BYTE_WIDTH = 2;                        // Ширина hex-представления байта
const size_t SHA256_HASH_LENGTH = SHA256_DIGEST_LENGTH; // Размер хеша SHA-256

// Контекст RSA подписи
const int RSA_PADDING_MODE = RSA_PKCS1_PADDING;

// Реализация методов хеширования
std::string CryptoUtils::calculateHash(const std::string &input)
{
    unsigned char hash[SHA256_HASH_LENGTH];

    // Генерация SHA-256 хеша
    SHA256(reinterpret_cast<const unsigned char *>(input.c_str()),
           input.size(),
           hash);

    std::stringstream ss;
    for (size_t i = 0; i < SHA256_HASH_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<int>(hash[i]);
    }

    return ss.str();
}

// Реализация методов цифровой подписи
std::string CryptoUtils::signData(const std::string &data, const std::string &privateKeyPEM)
{
    BIO *bio = nullptr;
    EVP_PKEY *pkey = nullptr;
    EVP_MD_CTX *ctx = nullptr;
    std::string result;

    try
    {
        // Этап 1: Загрузка приватного ключа из PEM-строки
        bio = BIO_new_mem_buf(privateKeyPEM.data(), -1);
        if (!bio)
            throw std::runtime_error("BIO_new_mem_buf failed");

        pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
        if (!pkey)
            throw std::runtime_error("PEM_read_bio_PrivateKey failed");

        BIO_free(bio);
        bio = nullptr;

        // Этап 2: Создание контекста подписи
        ctx = EVP_MD_CTX_new();
        if (!ctx)
            throw std::runtime_error("EVP_MD_CTX_new failed");

        // Инициализация алгоритма подписи
        if (1 != EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, pkey))
        {
            throw std::runtime_error("EVP_DigestSignInit failed");
        }

        // Этап 3: Вычисление подписи
        if (1 != EVP_DigestSignUpdate(ctx, data.data(), data.size()))
        {
            throw std::runtime_error("EVP_DigestSignUpdate failed");
        }

        // Определение размера подписи
        size_t sigLen = 0;
        if (1 != EVP_DigestSignFinal(ctx, nullptr, &sigLen))
        {
            throw std::runtime_error("EVP_DigestSignFinal (length) failed");
        }

        // Генерация окончательной подписи
        std::vector<unsigned char> sig(sigLen);
        if (1 != EVP_DigestSignFinal(ctx, sig.data(), &sigLen))
        {
            throw std::runtime_error("EVP_DigestSignFinal failed");
        }

        // Конвертация в HEX
        std::stringstream ss;
        for (size_t i = 0; i < sigLen; ++i)
        {
            ss << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<int>(sig[i]);
        }
        result = ss.str();
    }
    catch (const std::exception &e)
    {
        char openssl_error[256] = {0};
        ERR_error_string_n(ERR_get_error(), openssl_error, sizeof(openssl_error));
        ConsoleUI::printError(std::string(e.what()) + ": " + openssl_error);
        result.clear();
    }

    // Очистка ресурсов
    if (bio)
        BIO_free(bio);
    if (pkey)
        EVP_PKEY_free(pkey);
    if (ctx)
        EVP_MD_CTX_free(ctx);

    return result;
}

// Реализация верификации подписи
bool CryptoUtils::verifySignature(const std::string &data, const std::string &signatureHex, const std::string &publicKeyPEM)
{   
    // Этап 1: Конвертация HEX-подписи в бинарный формат
    std::vector<unsigned char> signature;
    for (size_t i = 0; i < signatureHex.length(); i += HEX_BYTE_WIDTH)
    {
        std::string byteString = signatureHex.substr(i, HEX_BYTE_WIDTH);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        signature.push_back(byte);
    }

    // Этап 2: Загрузка публичного ключа
    BIO *bio = BIO_new_mem_buf(publicKeyPEM.data(), -1);
    if (!bio)
    {
        ConsoleUI::printError("BIO_new_mem_buf failed");
        return false;
    }

    EVP_PKEY *pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!pkey) {
        ConsoleUI::printError("PEM_read_bio_PUBKEY failed");
        return false;
    }

    // Этап 3: Верификация подписи
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        EVP_PKEY_free(pkey);
        ConsoleUI::printError("EVP_MD_CTX_new failed");
        return false;
    }

    // Настройка алгоритма проверки
    if (EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pkey) <= 0)
    {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return false;
    }

    // Настройка алгоритма проверки
    bool verificationResult = false;
    do {
        if (EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pkey) <= 0) break;
        
        // Установка режима паддинга для RSA
        EVP_PKEY_CTX *pctx = EVP_MD_CTX_pkey_ctx(ctx);
        if (pctx) {
            EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PADDING_MODE);
        }

        if (EVP_DigestVerifyUpdate(ctx, data.data(), data.size()) <= 0) break;
        
        verificationResult = (1 == EVP_DigestVerifyFinal(ctx, 
            signature.data(), 
            signature.size()));
    } while (false);
    
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    return verificationResult;
}

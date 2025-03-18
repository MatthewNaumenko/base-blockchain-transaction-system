// BC_RSAKeyGenerator.cpp
#include "BC_RSAKeyGenerator.h"
#include "BC_Utilities.h"

// OpenSSL компоненты
#include <openssl/pem.h>

// Генерация пары RSA-ключей
EVP_PKEY *RSAKeyGenerator::generateRSAKeyPair(int keyLength)
{

    // Создаем контекст для генерации ключей
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx)
    {
        ConsoleUI::printError("EVP_PKEY_CTX_new_id failed");
        return nullptr;
    }

    // Инициализируем контекст для генерации ключей
    if (EVP_PKEY_keygen_init(ctx) <= 0)
    {
        ConsoleUI::printError("EVP_PKEY_keygen_init failed");
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Устанавливаем длину ключа
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, keyLength) <= 0)
    {
        ConsoleUI::printError("EVP_PKEY_CTX_set_rsa_keygen_bits failed");
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Генерация ключа
    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
    {
        ConsoleUI::printError("Key generation failed");
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }

    // Освобождаем контекст
    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

// Получение PEM-строки из приватного ключа
std::string RSAKeyGenerator::getPEMFromPrivateKey(EVP_PKEY *pkey)
{   
    if (!pkey)
    {
        ConsoleUI::printError("Invalid private key provided");
        return "";
    }

    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio)
    {
        ConsoleUI::printError("BIO_new failed");
        return "";
    }

    // Записываем приватный ключ в формате PEM в BIO
    if (!PEM_write_bio_PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr))
    {
        ConsoleUI::printError("PEM_write_bio_PrivateKey failed");
        BIO_free(bio);
        return "";
    }

    // Получаем данные из BIO
    char *pemData = nullptr;
    long pemLen = BIO_get_mem_data(bio, &pemData);
    std::string pemString(pemData, pemLen);

    // Освобождаем BIO
    BIO_free(bio);
    return pemString;
}

// Получение PEM-строки из публичного ключа
std::string RSAKeyGenerator::getPEMFromPublicKey(EVP_PKEY *pkey)
{   
    if (!pkey)
    {
        ConsoleUI::printError("Invalid public key provided");
        return "";
    }

    // Создаем BIO для работы с памятью
    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio)
    {
        ConsoleUI::printError("BIO_new failed");
        return "";
    }

    // Записываем публичный ключ в формате PEM в BIO
    if (!PEM_write_bio_PUBKEY(bio, pkey))
    {
        ConsoleUI::printError("PEM_write_bio_PUBKEY failed");
        BIO_free(bio);
        return "";
    }

    // Получаем данные из BIO
    char *pemData = nullptr;
    long pemLen = BIO_get_mem_data(bio, &pemData);
    std::string pemString(pemData, pemLen);

    // Освобождаем BIO
    BIO_free(bio);
    return pemString;
}

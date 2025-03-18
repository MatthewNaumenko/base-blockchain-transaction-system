// BC_Utilities.cpp
#include "BC_Utilities.h"

// Системные библиотеки (только для реализации)
#include <iostream>
#include <iomanip>
#include <sstream>

// OpenSSL компоненты
#include <openssl/evp.h>
#include <openssl/rand.h>

// Реализация методов ConsoleUI
void ConsoleUI::printBanner()
{
    std::cout << "\n";
    std::cout << "=======================================================\n";
    std::cout << "|      Base Blockchain Transaction System v1.0.0      |\n";
    std::cout << "|-----------------------------------------------------|\n";
    std::cout << "|  - SHA-256 Cryptographic Hashing                    |\n";
    std::cout << "|  - Secure Blockchain Transactions                   |\n";
    std::cout << "|  - Multi-threaded Mining                            |\n";
    std::cout << "|-----------------------------------------------------|\n";
    std::cout << "|  Developer: Matthew Naumenko                        |\n";
    std::cout << "|  License: Apache 2.0                                |\n";
    std::cout << "|  Contact: naumenko33301@gmail.com                   |\n";
    std::cout << "|-----------------------------------------------------|\n";
    std::cout << "|      # 2025 | Open Source Project | Build: 2406     |\n";
    std::cout << "=======================================================\n\n";
}

void ConsoleUI::printHeader(const std::string &title)
{
    std::cout << "\033[1;35m" << title << "\033[0m\n";
}
void ConsoleUI::printSectionHeader(const std::string &title)
{
    std::cout << "\n\033[1;34m=== " << title << " ===\033[0m\n";
}

void ConsoleUI::printDefault(const std::string &message, bool newLine)
{
    std::cout << message << (newLine ? "\n" : "");
}

void ConsoleUI::printMining(const std::string &message)
{
    std::cout << "\033[1;34m[MINING]\033[0m " << message << "\n";
}

void ConsoleUI::printSuccess(const std::string &message)
{
    std::cout << "\033[1;32m[SUCCESS] " << message << "\033[0m\n";
}

void ConsoleUI::printError(const std::string &message)
{
    std::cerr << "\033[1;31m[ERROR] " << message << "\033[0m\n";
}

void ConsoleUI::printWarning(const std::string &message)
{
    std::cout << "\033[1;33m[WARNING] " << message << "\033[0m\n";
}

void ConsoleUI::printInfo(const std::string &message, bool newLine)
{
    std::cout << "\033[1;36m[INFO]\033[0m  " << message << (newLine ? "\n" : "");
}

void ConsoleUI::printDivider(char symbol, int length)
{
    std::cout << std::string(length, symbol) << "\n";
}

void ConsoleUI::printMenuOptions(const std::vector<std::string> &options)
{
    for (const auto &opt : options)
    {
        std::cout << opt << "\n";
    }
}

void ConsoleUI::printMenu(std::string &user)
{
    printDivider('=');
    printHeader("Blockchain System Menu");
    printInfo("Logged in as: " + user);
    printDivider('-');
    printMenuOptions({"1. Register user",
                      "2. Show list of users",
                      "3. Select user",
                      "4. Create transaction",
                      "5. Show blockchain",
                      "6. Save blockchain to file",
                      "7. Validate blockchain",
                      "8. Exit"});
    printDivider('=');
    printDefault("Choose an action: ", false);
}

// Реализация методов TimeUtils
std::string TimeUtils::getCurrentTime()
{
    std::time_t now = std::time(nullptr);
    std::tm timeStruct;

#ifdef _WIN32
    localtime_s(&timeStruct, &now);
#else
    localtime_r(&now, &timeStruct);
#endif

    // Формат ISO 8601
    std::stringstream ss;
    ss << std::put_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Реализация методов Validator
bool Validator::isAddressFormatValid(const std::string &address)
{
    if (address.length() < 3 || address.length() > 20)
        return false;
    for (char c : address)
    {
        if (!std::isalnum(c) && c != '_')
            return false;
    }
    return true;
}

std::string PersistenceManager::encryptString(const std::string &plaintext, const std::string &key)
{   
    // Создаем контекст для шифрования
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create encryption context");
    }

    // Выбираем алгоритм шифрования AES-256-CBC
    const EVP_CIPHER *cipher = EVP_aes_256_cbc();

    // Генерируем случайный вектор инициализации (IV)
    int iv_length = EVP_CIPHER_iv_length(cipher);
    unsigned char iv[EVP_MAX_IV_LENGTH];
    RAND_bytes(iv, iv_length);

    // Вычисляем размер буфера для зашифрованного текста
    int ciphertext_len = static_cast<int>(plaintext.size()) + EVP_CIPHER_block_size(cipher);
    std::vector<unsigned char> ciphertext(ciphertext_len);
    
    // Инициализируем шифрование
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, reinterpret_cast<const unsigned char *>(key.data()), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption initialization failed");
    }

    // Шифруем данные
    int len;
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char *>(plaintext.data()), static_cast<int>(plaintext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption update failed");
    }
    int total_len = len;

    // Завершаем шифрование
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption finalization failed");
    }
    total_len += len;

    // Освобождаем контекст шифрования
    EVP_CIPHER_CTX_free(ctx);

    // Формируем результат: IV + зашифрованный текст
    std::string result(reinterpret_cast<char *>(iv), iv_length);
    result.append(reinterpret_cast<char *>(ciphertext.data()), total_len);

    return result;
}

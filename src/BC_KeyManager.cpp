// BC_KeyManager.cpp
#include "BC_KeyManager.h"
#include "BC_RSAKeyGenerator.h"
#include "BC_Utilities.h"

// Системные библиотеки (только для реализации)
#include <fstream>
#include <iostream>
#include <filesystem>

// OpenSSL компоненты
#include <openssl/evp.h>

namespace fs = std::filesystem;

KeyManager::KeyManager(const std::vector<std::string> &users)
{
    for (const auto &user : users)
    {
        generateAndSaveKeys(user);
    }
}

// Генерация пары RSA-ключей
void KeyManager::generateAndSaveKeys(const std::string &username)
{
    EVP_PKEY *keyPair = RSAKeyGenerator::generateRSAKeyPair();
    if (!keyPair)
    {
        ConsoleUI::printError("Failed to generate key for " + username);
        return;
    }

    ConsoleUI::printSuccess("Private key generate success for " + username);
    
    // Получение PEM-представления ключей
    std::string publicKeyPEM = RSAKeyGenerator::getPEMFromPublicKey(keyPair);
    std::string privateKeyPEM = RSAKeyGenerator::getPEMFromPrivateKey(keyPair);

    // Сохраняем публичный ключ
    publicKeys[username] = publicKeyPEM;

    // Создание папки keys, если она не существует
    fs::path keysDir = PROJECT_ROOT "/keys";
    if (!fs::exists(keysDir))
    {
        if (!fs::create_directory(keysDir))
        {
            ConsoleUI::printError("Failed to create directory 'keys'");
            EVP_PKEY_free(keyPair);
            return;
        }
        ConsoleUI::printWarning("Directory 'keys' created successfully.");
    }

    // Сохранение приватного ключа
    fs::path privatePath = keysDir / (username + "_private.pem");
    std::ofstream privateFile(privatePath);
    if (privateFile)
    {
        privateFile << privateKeyPEM;
        privateFile.close();
        ConsoleUI::printWarning("Private key saved to: " + privatePath.string());
    }
    else
    {
        ConsoleUI::printError("Failed to save private key for " + username);
    }

    ConsoleUI::printDefault("Your private key (truncated):\n" + truncateKey(privateKeyPEM));

    // Очищаем приватный ключ из памяти
    if (!privateKeyPEM.empty())
    {
        // Заполняем строку нулями
        std::memset(&privateKeyPEM[0], 0, privateKeyPEM.size());
        privateKeyPEM.clear(); // Очищаем содержимое строки
    }

    // Освобождение памяти
    EVP_PKEY_free(keyPair);
}

// Добавляет ключи для нового пользователя
void KeyManager::addUserKeys(const std::string &username)
{
    if (publicKeys.count(username) > 0)
    {
        throw std::runtime_error("User already exists");
    }
    generateAndSaveKeys(username);
}

// Возвращает карту публичных ключей
std::map<std::string, std::string> &KeyManager::getPublicKeys()
{
    return publicKeys;
}

// Укорачивает строку ключа
std::string KeyManager::truncateKey(const std::string &key, int headLen, int tailLen)
{
    if (key.length() <= headLen + tailLen)
    {
        return key;
    }
    return key.substr(0, headLen) + "................." + key.substr(key.length() - tailLen);
}
// BC_Controller.cpp
#include "BC_Controller.h"
#include "BC_Transaction.h"
#include "BC_Blockchain.h"
#include "BC_Utilities.h"
#include "BC_Block.h"

// Системные библиотеки (только для реализации)
#include <iostream>
#include <fstream>

BlockchainController::BlockchainController(const std::map<std::string, std::string> &pubKeys)
    : publicKeys(pubKeys) {}

// Обрабатывает список транзакций: подписывает их и добавляет в новый блок
void BlockchainController::processTransactions(std::vector<Transaction> transactions)
{
    blockchain.addBlock(transactions, publicKeys);
}

// Проверяет, валиден ли текущий блокчейн
bool BlockchainController::isBlockchainValid() const
{
    return blockchain.isChainValid(publicKeys);
}

// Выводит блокчейн в консоль и отрисовывает его структуру
void BlockchainController::printBlockchain() const
{
    blockchain.printBlockchain();
    blockchain.drawChain();
}

// Сохраняет блокчейн в файл с использованием BlockchainController
void BlockchainController::saveBlockchain(const std::string &filename, const std::string &key)
{
    saveBlockchainToFile(blockchain, filename, key);
}

// Регистрирует нового пользователя в блокчейне
void BlockchainController::registerUser(const std::string &username)
{
    blockchain.addUser(username);
}

// Возвращает баланс пользователя по его имени
double BlockchainController::getUserBalance(const std::string &username) const
{
    return blockchain.getBalance(username);
}

// Логика сохранения блокчейна в файл, шифруя данные с использованием ключа
void BlockchainController::saveBlockchainToFile(const Blockchain &save_blockchain, const std::string &filename, const std::string &key)
{
    std::string blockchainData = save_blockchain.serialize();
    std::string encryptedData = PersistenceManager::encryptString(blockchainData, key);
    
    std::ofstream ofs(filename, std::ios::binary);
    if (ofs)
    {
        ofs.write(encryptedData.data(), encryptedData.size());
        ofs.close();
        ConsoleUI::printSuccess("Blockchain saved to: " + filename);
    }
    else
    {
        ConsoleUI::printError("Failed to open file for writing");
    }
}

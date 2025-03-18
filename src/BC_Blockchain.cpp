// BC_Blockchain.cpp
#include "BC_Blockchain.h"
#include "BC_Block.h"
#include "BC_Transaction.h"
#include "BC_CryptoUtils.h"
#include "BC_Utilities.h"

// Системные библиотеки (только для реализации)
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <sstream>
#include <algorithm>

// Создание генезис-блока
Block Blockchain::createGenesisBlock() {
    balances["Genesis_User"] = 1000;
    Transaction genesisTx("System", "Genesis_User", 1000);
    
    return Block(0, 
               "0", 
               {genesisTx}, 
               balances, 
               4); // Стартовая сложность майнинга
}

Blockchain::Blockchain()
{
    chain.push_back(createGenesisBlock());
}

// Управление пользователями
void Blockchain::addUser(const std::string &username)
{
    std::lock_guard<std::mutex> lock(balanceMutex);



    if (balances.count(username) == 0)
    {
        if (Validator::isAddressFormatValid(username))
        {
            balances[username] = 0;
        }
        else
        {
            ConsoleUI::printError("Invalid username format: " + username);
        }
    }
    else
    {
        ConsoleUI::printError("User already exists: " + username);
    }
}

// Работа с транзакциями
bool Blockchain::isTransactionValid(const Transaction& tx,
    const std::string& publicKeyPEM,
    std::map<std::string, double>& tempBalances) const 
{   
    // Системные транзакции не требуют проверки
    if (tx.getSender() == "System") return true;

    // Базовые проверки данных

    if (tx.getSignature().empty())
    {
        ConsoleUI::printError("Missing signature for TX: " + tx.getTxId());
        return false;
    }

    if (tempBalances.count(tx.getSender()) == 0 || tempBalances[tx.getSender()] < tx.getAmount())
    {
        ConsoleUI::printError("Insufficient balance for sender: " + tx.getSender());
        return false;
    }

    if (tx.getAmount() < 0)
    {
        ConsoleUI::printError("Negative transaction amount for TX: " + tx.getTxId());
        return false;
    }


    if (tx.getReceiver().empty())
    {
        ConsoleUI::printError("Empty receiver for TX: " + tx.getTxId());
        return false;
    }

    // Валидация адресов
    if (!Validator::isAddressFormatValid(tx.getReceiver()))
    {
        ConsoleUI::printWarning("Receiver address '" + tx.getReceiver() + "' is invalid. Funds may be lost.");
    }

    // Верификация криптографической подписи
    std::string dataToVerify = tx.getTxId() + tx.getSender() 
                               + tx.getReceiver() + std::to_string(tx.getAmount()) 
                               + tx.getTimestamp() + tx.getMetadata();

    if (!CryptoUtils::verifySignature(dataToVerify, tx.getSignature(), publicKeyPEM))
    {
        ConsoleUI::printError("Signature INVALID for TX: " + tx.getTxId());
        return false;
    }
    else
    {
        ConsoleUI::printSuccess("Signature valid for TX: " + tx.getTxId());
    }

    return true;
}


// Добавление блоков
void Blockchain::addBlock(const std::vector<Transaction> &transactions, 
                                        const std::map<std::string, 
                                        std::string> &publicKeys)
{
    std::lock_guard<std::mutex> lock(balanceMutex);
    std::map<std::string, double> tempBalances = balances;

    // Предварительная обработка транзакций
    for (const auto &tx : transactions)
    {

        // Поиск публичного ключа отправителя
        auto it = publicKeys.find(tx.getSender());
        if (it == publicKeys.end())
        {
            ConsoleUI::printError("Public key not found for sender: " + tx.getSender());
            ConsoleUI::printError("Block not added.");
            return;
        }

        // Валидация получателя
        if (!Validator::isAddressFormatValid(tx.getReceiver()))
        {
            ConsoleUI::printError("Invalid receiver address: " + tx.getReceiver());
            return;
        }

        // Валидация транзакции
        if (!isTransactionValid(tx, it->second, tempBalances))
        {
            ConsoleUI::printError("Transaction " + tx.getTxId() + " is invalid. Block not added.");
            return;
        }

        // Обновление временных балансов
        tempBalances[tx.getSender()] -= tx.getAmount();
        tempBalances[tx.getReceiver()] += tx.getAmount(); // Автоматически создает запись, если получателя нет

        // Авторегистрация новых пользователей
        if (balances.count(tx.getReceiver()) == 0)
        {
            ConsoleUI::printWarning("Receiver " + tx.getReceiver() + " not registered! Automatically creating account.");
            balances[tx.getReceiver()] = 0; 
        }
    }

    // Фильтрация нулевых балансов
    for (auto it = tempBalances.begin(); it != tempBalances.end();)
    {
        if (it->second == 0 && balances.find(it->first) == balances.end())
        {
            it = tempBalances.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Сохранение актуальных балансов
    balances = tempBalances;

    // Фильтрация балансов перед сохранением в блок
    std::map<std::string, double> snapshot;
    for (const auto &[user, balance] : balances)
    {
        // Включаем только участников транзакций или с ненулевым балансом
        bool isInvolved = std::any_of(transactions.begin(), transactions.end(),
                                      [&](const Transaction &tx)
                                      {
                                          return tx.getSender() == user || tx.getReceiver() == user;
                                      });

        if (balance != 0 || isInvolved)
        {
            snapshot[user] = balance;
        }
    }

    // Создание и добавление нового блока
    Block latestBlock = getLatestBlock();
    Block newBlock(latestBlock.getIndex() + 1,
                   latestBlock.getHash(),
                   transactions,
                   snapshot,
                   latestBlock.getDifficulty());

    ConsoleUI::printInfo("Balance snapshot for block " + std::to_string(newBlock.getIndex()));
    for (const auto &[user, balance] : snapshot)
    {
        ConsoleUI::printDefault("  " + user + ": " + std::to_string(balance));
    }

    chain.push_back(newBlock);
    ConsoleUI::printSuccess("Transaction successfully added to blockchain!");
}

// Валидация цепочки
bool Blockchain::isChainValid(const std::map<std::string, std::string> &publicKeys) const
{
    ConsoleUI::printInfo("[Blockchain Validation] Starting...");
    ConsoleUI::printInfo("Total blocks to validate: " + std::to_string(chain.size()) + "\n");

    std::map<std::string, double> tempBalances;
    bool isValid = true;

    for (size_t i = 0; i < chain.size(); ++i)
    {
        const Block &current = chain[i];
        ConsoleUI::printDefault("Checking Block #" + std::to_string(current.getIndex()) 
                                        + " (Hash: " + current.getHash().substr(0, 12) 
                                        + "..." + current.getHash().substr(56) + ")");

        if (i > 0)
        {
            tempBalances = chain[i - 1].getBalanceSnapshot();
        }

        // Проверка Proof-of-Work
        ConsoleUI::printDefault("Checking Proof-of-Work...", false);
        if (current.getHash().substr(0, current.getDifficulty()) == std::string(current.getDifficulty(), '0'))
        {
            ConsoleUI::printDefault("Valid (Difficulty: " + std::to_string(current.getDifficulty()) 
                                    + ", Leading zeros: " + current.getHash().substr(0, current.getDifficulty()) + ")");
        }
        else
        {
            ConsoleUI::printDefault("Invalid! First " + std::to_string(current.getDifficulty()) 
                                    + " chars: " + current.getHash().substr(0, current.getDifficulty()));
            isValid = false;
        }

        // Проверка хеша блока
        ConsoleUI::printDefault("Checking block hash... ", false);
        if (current.getHash() == current.calculateBlockHash())
        {
            ConsoleUI::printDefault("Valid");
        }
        else
        {
            ConsoleUI::printDefault("Invalid!");
            isValid = false;
        }

        // Проверка связи с предыдущим блоком
        if (i > 0)
        {
            ConsoleUI::printDefault("Checking chain link... ", false);
            if (current.getPreviousHash() == chain[i - 1].getHash())
            {
                ConsoleUI::printDefault("Valid (Prev hash: " + chain[i - 1].getHash().substr(0, 12) + "...)");
            }
            else
            {
                ConsoleUI::printDefault("Broken link! Expected: " + chain[i - 1].getHash().substr(0, 12) 
                                        + "...\n" + "                  Actual: " + current.getPreviousHash().substr(0, 12) + "...");
                isValid = false;
            }
        }

        // Проверка транзакций
        ConsoleUI::printDefault("Transactions (" + std::to_string(current.getTransactions().size()) + "):");
        for (const auto &tx : current.getTransactions())
        {
            ConsoleUI::printDefault("TX " + tx.getTxId().substr(0, 8) + "... | " + std::to_string(tx.getAmount()) 
                                    + " BTC " + tx.getSender().substr(0, 5) + " - " + tx.getReceiver().substr(0, 5) + " | ", false);

            if (tempBalances.find(tx.getReceiver()) == tempBalances.end())
            {
                tempBalances[tx.getReceiver()] = 0;
            }

            if (tx.getSender() == "System")
            {
                ConsoleUI::printDefault("System transaction (skipped checks)");
                tempBalances[tx.getReceiver()] += tx.getAmount();
                continue;
            }

            // Проверка подписи
            auto it = publicKeys.find(tx.getSender());
            if (it == publicKeys.end())
            {
                ConsoleUI::printDefault("Missing public key!");
                isValid = false;
                continue;
            }
            else
            {
                ConsoleUI::printDefault("Public key VALID!");
            }

            std::string dataToVerify = tx.getTxId() + tx.getSender() + tx.getReceiver() 
                                       + std::to_string(tx.getAmount()) 
                                       + tx.getTimestamp() + tx.getMetadata();

            if (CryptoUtils::verifySignature(dataToVerify, tx.getSignature(), it->second))
            {
                ConsoleUI::printDefault("Valid sig | ", false);
            }
            else
            {
                ConsoleUI::printDefault("Invalid sig | ", false);
                isValid = false;
            }

            // Проверка баланса
            if (tempBalances[tx.getSender()] >= tx.getAmount())
            {
                ConsoleUI::printDefault("Balance OK (" + std::to_string(tempBalances[tx.getSender()]) 
                                        + " - " + std::to_string(tempBalances[tx.getSender()] - tx.getAmount()) + ")");
            }
            else
            {
                ConsoleUI::printDefault("Insufficient funds for sender: " + tx.getSender());
                ConsoleUI::printDefault("Expected balances:");
                for (const auto &[k, v] : current.getBalanceSnapshot())
                {
                    ConsoleUI::printDefault("  " + k + ": " + std::to_string(v));
                }
                ConsoleUI::printDefault("Actual balances:");
                for (const auto &[k, v] : tempBalances)
                {
                    ConsoleUI::printDefault("  " + k + ": " + std::to_string(v));
                }
                ConsoleUI::printDefault("Available balance: " + std::to_string(tempBalances[tx.getSender()]));
                isValid = false;
            }

            // Обновление баланса
            tempBalances[tx.getSender()] -= tx.getAmount();
            tempBalances[tx.getReceiver()] += tx.getAmount();
        }

        // Создание отфильтрованного снапшота
        std::map<std::string, double> filteredTemp;
        for (const auto &[user, balance] : tempBalances)
        {
            bool isInvolved = std::any_of(
                current.getTransactions().begin(),
                current.getTransactions().end(),
                [&](const Transaction &tx)
                {
                    return tx.getSender() == user || tx.getReceiver() == user;
                });

            if (balance != 0 || isInvolved)
            {
                filteredTemp[user] = balance;
            }
        }

        // Очищаем нулевые балансы не участников
        for (auto it = filteredTemp.begin(); it != filteredTemp.end();)
        {
            if (it->second == 0 &&
                std::none_of(current.getTransactions().begin(),
                             current.getTransactions().end(),
                             [&](const Transaction &tx)
                             {
                                 return tx.getSender() == it->first || tx.getReceiver() == it->first;
                             }))
            {
                it = filteredTemp.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Проверяем снапшот блока против отфильтрованных данных
        ConsoleUI::printDefault("Checking balance snapshot... ", false);
        if (current.getBalanceSnapshot() == filteredTemp)
        {
            ConsoleUI::printDefault("Matched");
        }
        else
        {
            ConsoleUI::printDefault("Mismatch!");
            ConsoleUI::printDefault("Expected balances (from block):");
            for (const auto &[k, v] : current.getBalanceSnapshot())
            {
                ConsoleUI::printDefault("  " + k + ": " + std::to_string(v) + "\n", false);
            }
            ConsoleUI::printDefault("Actual filtered balances:");
            for (const auto &[k, v] : filteredTemp)
            {
                ConsoleUI::printDefault("  " + k + ": " + std::to_string(v) + "\n", false);
            }
            isValid = false;
        }

        // Обновляем для следующего блока
        tempBalances = filteredTemp;

        ConsoleUI::printDivider();
    }

    ConsoleUI::printDefault("\nValidation " + (isValid ? std::string("SUCCESSFUL") : std::string("FAILED")) 
                            + " | Blocks: " + std::to_string(chain.size()) + " | Total TX: " 
                            + std::to_string(countAllTransactions()) + "\n\n");

    return isValid;
}

// Вспомогательные методы

Block Blockchain::getLatestBlock() const
{
    return chain.back();
}

std::string Blockchain::serialize() const
{
    std::stringstream ss;
    for (const auto &block : chain)
    {
        ss << "Index: " << block.getIndex() << "\n";
        ss << "Timestamp: " << block.getTimestamp() << "\n";
        ss << "Transactions:\n";
        for (const auto &tx : block.getTransactions())
        {
            ss << "  - " << tx.toString() << "\n";
        }
        ss << "Previous Hash: " << block.getPreviousHash() << "\n";
        ss << "Hash: " << block.getHash() << "\n";
        ss << "--------------------------\n";
    }
    return ss.str();
}

size_t Blockchain::countAllTransactions() const
{
    size_t count = 0;
    for (const auto &block : chain)
    {
        count += block.getTransactions().size();
    }
    return count;
}

void Blockchain::printBlockchain() const
{
    for (const auto &block : chain)
    {
        block.printBlock();
    }
}

// Метод для получения баланса конкретного пользователя
double Blockchain::getBalance(const std::string &username) const
{
    auto it = balances.find(username);
    if (it != balances.end())
        return it->second;
    else
        return 0;
}

void Blockchain::drawChain() const
{
    ConsoleUI::printInfo("Visualization of the BlockChain:\n");

    std::string topBorder;
    for (size_t i = 0; i < chain.size(); i++)
    {
        topBorder += "  +-----------+  ";
        if (i != chain.size() - 1)
        {
            topBorder += "     ";
        }
    }
    ConsoleUI::printDefault(topBorder);

    std::string middlePart;
    for (size_t i = 0; i < chain.size(); i++)
    {
        std::stringstream ss;
        ss << "Block " << chain[i].getIndex();
        std::string blockStr = ss.str();
        int padding = (11 - static_cast<int>(blockStr.size())) / 2;

        std::string blockContent = "  |";
        for (int j = 0; j < padding; j++)
            blockContent += " ";
        blockContent += blockStr;
        for (int j = 0; j < 11 - padding - static_cast<int>(blockStr.size()); j++)
            blockContent += " ";
        blockContent += "|  ";

        if (i != chain.size() - 1)
        {
            blockContent += "---->";
        }

        middlePart += blockContent;
    }
    ConsoleUI::printDefault(middlePart);

    std::string bottomBorder;
    for (size_t i = 0; i < chain.size(); i++)
    {
        bottomBorder += "  +-----------+  ";
        if (i != chain.size() - 1)
        {
            bottomBorder += "     ";
        }
    }
    ConsoleUI::printDefault(bottomBorder + "\n");
}
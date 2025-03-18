// BC_Block.cpp
#include "BC_Block.h"
#include "BC_Transaction.h"
#include "BC_CryptoUtils.h"
#include "BC_Utilities.h"

// Системные библиотеки (только для реализации)
#include <thread>
#include <atomic>
#include <sstream>

// Реализация методов Block
Block::Block(int idx, const std::string &prevHash, const std::vector<Transaction> &txs,
             const std::map<std::string, double> &snapshot, int diff)
    : index(idx),
      timestamp(TimeUtils::getCurrentTime()),
      transactions(txs),
      previousHash(prevHash),
      nonce(0),
      balanceSnapshot(snapshot),
      difficulty(diff)
{
    hash = calculateBlockHash(); // Пересчёт хеша после инициализации всех полей
    mineBlock(difficulty);
}

void Block::mineBlock(int mine_difficulty)
{
    std::string target(mine_difficulty, '0');
    std::atomic<bool> found(false);
    std::atomic<int> atomicNonce(0);
    const unsigned int numThreads = std::thread::hardware_concurrency();
    int printInterval = 60000;

    ConsoleUI::printMining("Starting Proof-of-Work mining with " + std::to_string(numThreads) + " threads...");

    auto mining_task = [&]()
    {
        while (!found.load(std::memory_order_acquire))
        {
            const int currentNonce = atomicNonce.fetch_add(1, std::memory_order_relaxed);
            const std::string currentHash = calculateBlockHashWithNonce(currentNonce);

            if (currentNonce % printInterval == 0)
            {
                ConsoleUI::printMining("Thread " + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +
                                       " - nonce: " + std::to_string(currentNonce) +
                                       ", hash: " + currentHash);
            }

            if (currentHash.substr(0, mine_difficulty) == target)
            {
                nonce = currentNonce;
                hash = currentHash;
                found.store(true, std::memory_order_release);
                ConsoleUI::printMining("Block mined: " + hash +
                                       " (nonce: " + std::to_string(nonce) +
                                       ") by thread " + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())));
                break;
            }
        }
    };

    // Запуск пула потоков майнинга
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < numThreads; i++)
    {
        threads.emplace_back(mining_task);
    }
    for (auto &th : threads)
    {
        th.join();
    }
}

std::string Block::calculateBlockHashWithNonce(int testNonce) const
{
    std::stringstream hashStream;

    hashStream << index
               << timestamp
               << previousHash
               << testNonce;

    // Включение хешей всех транзакций
    for (const auto &tx : transactions)
    {
        hashStream << tx.toString();
    }

    return CryptoUtils::calculateHash(hashStream.str());
}

std::string Block::calculateBlockHash() const
{
    return calculateBlockHashWithNonce(nonce);
}

void Block::printBlock() const
{
    std::ostringstream ss;
    ss << "+----------------------------------+\n"
       << "|           BEGIN BLOCK INFO       |\n"
       << "+----------------------------------+\n"
       << "| Index:        " << index << "\n"
       << "| Timestamp:    " << timestamp << "\n"
       << "+----------------------------------+\n"
       << "| Transactions: \n";

    for (const auto &tx : transactions)
    {
        ss << "|   - " << tx.toString() << "\n";
    }

    ss << "+----------------------------------+\n"
       << "| Previous Hash: \n| " << previousHash << "\n"
       << "| Hash: \n| " << hash << "\n"
       << "+----------------------------------+\n"
       << "|           END BLOCK INFO         |\n"
       << "+----------------------------------+\n";

    ConsoleUI::printDefault(ss.str());
}

// Геттеры
const std::string &Block::getTimestamp() const { return timestamp; }
const std::string &Block::getHash() const { return hash; }
const std::string &Block::getPreviousHash() const { return previousHash; }
const std::vector<Transaction> &Block::getTransactions() const { return transactions; }
const int &Block::getIndex() const { return index; }
const int &Block::getDifficulty() const { return difficulty; }
const std::map<std::string, double> &Block::getBalanceSnapshot() const { return balanceSnapshot; }
// BC_Block.h
#pragma once

// Системные библиотеки
#include <string>
#include <vector>
#include <map>
#include <mutex>

// Forward declarations
class Transaction;
class CryptoUtils;
class ConsoleUI;
class TimeUtils;

/**
 * @brief Класс-посредник для управления блокчейном.
 *
 * Класс BlockchainController предоставляет методы для обработки транзакций,
 * проверки валидности блокчейна, регистрации пользователей и работы с балансами.
 */
class Block
{
private:
    int index;                                          ///< Порядковый номер блока в цепочке
    std::string timestamp;                              ///< Время создания блока (ISO 8601)
    std::vector<Transaction> transactions;              ///< Список содержащихся транзакций
    std::string previousHash;                           ///< Хеш предыдущего блока в цепочке
    std::string hash;                                   ///< Хеш текущего блока (SHA-256)
    int nonce;                                          ///< Число для доказательства работы
    std::map<std::string, double> balanceSnapshot;      ///< Снимок балансов на момент создания
    int difficulty;                                     ///< Текущая сложность майнинга

public:
    /**
     * @brief Конструктор блока
     * @param idx Индекс блока в цепочке (> 0)
     * @param prevHash Валидный хеш предыдущего блока
     * @param txs Вектор верифицированных транзакций
     * @param snapshot Снимок балансов кошельков
     * @param diff Требуемое количество ведущих нулей в хеше
     */
    Block(int idx, const std::string &prevHash,
          const std::vector<Transaction> &txs,
          const std::map<std::string, double> &snapshot,
          int diff);
    
    /// @name Геттеры
    /// @{
    const std::string &getTimestamp() const;                         ///< Время создания блока
    const std::string &getHash() const;                              ///< Текущий хеш блока
    const std::string &getPreviousHash() const;                      ///< Хеш предыдущего блока
    const std::vector<Transaction> &getTransactions() const;         ///< Доступ к транзакциям
    const int &getIndex() const;                                     ///< Позиция в блокчейне
    const int &getDifficulty() const;                                ///< Сложность майнинга
    const std::map<std::string, double> &getBalanceSnapshot() const; ///< Состояние балансов
    /// @}
    
    /**
     * @brief Процесс майнинга блока (Proof-of-Work)
     * @param difficulty Требуемое количество ведущих нулей
     * @throw std::runtime_error При ошибках майнинга
    */
    void mineBlock(int difficulty);
    
    /**
     * @brief Вычисляет текущий хеш блока на основе приватного метода calculateBlockHashWithNonce
     * @return HEX-строка с хешем (длиной 64 символа)
    */   
    std::string calculateBlockHash() const;

    /**
     * @brief Выводит форматированную информацию о блоке
     * @details Формат включает:
     * - Индекс и временную метку
     * - Хеши текущего и предыдущего блоков
     * - Список транзакций в сокращенном формате
     */    
    void printBlock() const;

private:
    /**
     * @brief Вспомогательный метод для вычисления хеша с указанным nonce
     * @param testNonce Тестовое значение для подбора
     * @return Промежуточный хеш для проверки сложности
     */
    std::string calculateBlockHashWithNonce(int testNonce) const;
};


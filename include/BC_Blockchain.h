// BC_Blockchain.h
#pragma once

// Системные библиотеки
#include <string>
#include <vector>
#include <map>
#include <mutex>

// Forward declarations
class Block;
class Transaction;
class CryptoUtils;
class ConsoleUI;
class TimeUtils;
class Validator;

/**
 * @brief Ядро блокчейн-системы, управляющее цепочкой блоков и балансами.
 * 
 * Обеспечивает:
 * - Создание и верификацию блоков
 * - Управление балансами пользователей
 * - Механизмы добавления транзакций
 * - Потокобезопасные операции с использованием мьютексов
 */
class Blockchain
{
private:
    std::vector<Block> chain;                   ///< Основная цепочка блоков
    std::map<std::string, double> balances;     ///< Текущие балансы пользователей
    std::mutex balanceMutex;                    ///< Синхронизация доступа к балансам

    /// @brief Создает начальный (генезис) блок системы
    Block createGenesisBlock();

public:
    /**
     * @brief Инициализирует блокчейн с генезис-блоком
     */
    Blockchain();

    /**
     * @brief Регистрирует нового пользователя в системе
     * @param username Имя пользователя (3-20 символов, буквы(анг)/цифры/_)
     * @throw std::invalid_argument При неверном формате имени
     */
    void addUser(const std::string &username);

    /**
     * @brief Возвращает последний добавленный блок
     * @warning Не потокобезопасен - должен вызываться внутри синхронизированных блоков
     */    
    Block getLatestBlock() const;

    /**
     * @brief Сериализует блокчейн в читаемый текстовый формат
     * @return Строка с полным описанием всех блоков
     */
    std::string serialize() const;

    /**
     * @brief Проверяет валидность транзакции
     * @param tx Проверяемая транзакция
     * @param publicKeyPEM Публичный ключ отправителя в PEM-формате
     * @param tempBalances Временные балансы для проверки
     * @return true если транзакция корректна
     * 
     * Выполняет:
     * - Проверку подписи
     * - Валидацию форматов
     * - Контроль баланса
     */
    bool isTransactionValid(const Transaction &tx, const std::string &publicKeyPEM, std::map<std::string, double> &tempBalances) const;

    /**
     * @brief Добавляет новый блок с транзакциями
     * @param transactions Вектор верифицированных транзакций
     * @param publicKeys Соответствие пользователей и их публичных ключей
     * 
     * Выполняет:
     * - Пакетную проверку транзакций
     * - Обновление балансов
     * - Создание снапшота системы
     * - Майнинг нового блока
     */
    void addBlock(const std::vector<Transaction> &transactions, const std::map<std::string, std::string> &publicKeys);

    /**
     * @brief Проверяет целостность всей цепочки
     * @param publicKeys Публичные ключи всех участников
     * @return true если все блоки и транзакции валидны
     * 
     * Проверяет:
     * - Proof-of-Work каждого блока
     * - Целостность хешей
     * - Корректность подписей транзакций
     * - Историческую согласованность балансов
     */
    bool isChainValid(const std::map<std::string, std::string> &publicKeys) const;
    
    /// @brief Возвращает общее количество транзакций в цепочке
    size_t countAllTransactions() const;

    /// @brief Выводит информацию о всех блоках в консоль
    void printBlockchain() const;

    /**
     * @brief Возвращает текущий баланс пользователя
     * @param username Имя целевого пользователя
     * @return Текущий баланс (0 если пользователь не существует)
     */
    double getBalance(const std::string &username) const;
    
    /// @brief Отображает ASCII-визуализацию цепочки блоков
    void drawChain() const;
};
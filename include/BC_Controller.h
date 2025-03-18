// BC_Controller.h
#pragma once

// Системные библиотеки
#include <map>
#include <string>
#include <vector>

#include "BC_Blockchain.h"

// Forward declarations
class Transaction;

/**
 * @brief Класс-посредник для управления блокчейном.
 *
 * Класс-посредник BlockchainController управляет блокчейном и предоставляет методы для обработки транзакций,
 * проверки валидности блокчейна, регистрации пользователей и работы с балансами.
 */
class BlockchainController
{
private: 
    Blockchain blockchain;                                  ///< Объект блокчейна
    const std::map<std::string, std::string> &publicKeys;   ///< Ссылка на карту публичных ключей пользователей

public:
    /**
     * Конструктор класса BlockchainController.
     * @param pubKeys Карта публичных ключей пользователей.
     */
    BlockchainController(const std::map<std::string, std::string> &pubKeys);

    /**
     * Обрабатывает список транзакций: подписывает их и добавляет в новый блок.
     * @param transactions Список транзакций для обработки.
     */
    void processTransactions(std::vector<Transaction> transactions);

    /**
     * Проверяет валидность блокчейна.
     * @return true, если блокчейн валиден, иначе false.
     */
    bool isBlockchainValid() const;

    /**
     * Выводит блокчейн в консоль и отрисовывает его структуру.
     */
    void printBlockchain() const;

    /**
     * Сохраняет блокчейн в файл через saveBlockchainToFile.
     * @param filename Имя файла для сохранения.
     * @param key Ключ для шифрования данных.
     */
    void saveBlockchain(const std::string &filename, const std::string &key);

    /**
     * Регистрирует нового пользователя в блокчейне.
     * @param username Имя пользователя для регистрации.
     */
    void registerUser(const std::string &username);

    /**
     * Возвращает баланс пользователя по его имени.
     * @param username Имя пользователя.
     * @return Баланс пользователя.
     */
    double getUserBalance(const std::string &username) const;

private: 
    /**
     * Сохраняет блокчейн в файл, шифруя его с использованием ключа.
     * @param blockchain Объект блокчейна для сохранения.
     * @param filename Имя файла для сохранения.
     * @param key Ключ для шифрования данных.
     */
    void saveBlockchainToFile(const Blockchain &blockchain,
                                                    const std::string &filename, const std::string &key);
};
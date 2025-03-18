// BC_Transaction.h
#pragma once

// Системные библиотеки
#include <string>

/**
 * @class Transaction
 * @brief Класс, представляющий криптографически защищенную транзакцию блокчейна
 * 
 * Инкапсулирует логику валидации входных данных, генерации цифровой подписи 
 * и сериализации транзакции. Все операции выполняют предварительные проверки 
 * целостности данных.
 */
class Transaction
{
private:
    std::string sender;     ///< Блокчейн-адрес отправителя (валидируется при создании)
    std::string receiver;   ///< Блокчейн-адрес получателя (валидируется при создании)
    double amount;          ///< Сумма перевода (>0, иначе инициализируется 0)
    std::string txId;       ///< Уникальный SHA256-хеш (sender+receiver+amount+timestamp+metadata)
    std::string timestamp;  ///< Временная метка в ISO 8601 от TimeUtils
    std::string metadata;   ///< Произвольные данные (обрезаются до 512 символов при инициализации)
    std::string signature;  ///< ECDSA-подпись в формате PEM (генерируется signTransaction)

    /**
     * @brief Генерирует детерминированные данные для подписи
     * @return Конкатенация полей в порядке: txId + sender + receiver + 
     *         std::to_string(amount) + timestamp + metadata
     * @warning Изменение формата приведет к ошибкам верификации существующих подписей
     */
    std::string getDataToSign() const;

public:
    /**
     * @brief Создает транзакцию с базовой валидацией полей
     * @param from Адрес отправителя (только валидные Base58Check-адреса)
     * @param to Адрес получателя (аналогично from)
     * @param value Положительная сумма перевода (<=0 инициализирует 0)
     * @param meta Метаданные (обрезаются до 512 символов, могут быть пустыми)
     * @throws implicit: неявная инициализация невалидных полей пустыми значениями/нулем
     * @note Автоматически генерирует txId и timestamp через CryptoUtils и TimeUtils
     */
    Transaction(const std::string &from, const std::string &to, double value, const std::string &meta = "");

    /**
     * @brief Выполняет криптографическое подписание транзакции
     * @param privateKeyPEM Приватный ключ в PEM-формате с заголовками
     * @throws std::runtime_error При: повторном подписании, невалидных полях транзакции,
     *         ошибках криптографических операций, несоответствии формата ключа
     * @note Логирует процесс через ConsoleUI::printInfo. Требует предварительной
     *       инициализации всех полей транзакции (кроме signature)
     */
    void signTransaction(const std::string &privateKeyPEM);

    /**
     * @brief Сериализует транзакцию в читаемый формат
     * @return Строка вида "txId: ..., From: ..., To: ..., Amount: X.XXXX, ..."
     * @note Включает все поля кроме подписи. Для подписи использует getSignature()
     * @warning Формат вывода может меняться
     */
    std::string toString() const;

    /// @{ Группа методов только для чтения (поля защищены от прямой модификации)
    const std::string &getTxId() const;         ///< Хеш-идентификатор транзакции
    const std::string &getSender() const;       ///< Нормализованный адрес отправителя
    const std::string &getSignature() const;    ///< Подпись в PEM или пустая строка
    const std::string &getReceiver() const;     ///< Нормализованный адрес получателя
    const std::string &getTimestamp() const;    ///< Временная метка создания транзакции
    const std::string &getMetadata() const;     ///< Метаданные (обрезанная версия)
    const double &getAmount() const;            ///< Фактически сохраненная сумма
    /// @}
};
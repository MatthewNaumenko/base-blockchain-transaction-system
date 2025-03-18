// BC_CryptoUtils.h
#pragma once

// Системные библиотеки
#include <string>

/**
 * @brief Класс для криптографических операций с использованием OpenSSL.
 * 
 * Предоставляет методы для:
 * - вычисления SHA-256 хеша,
 * - подписи данных с помощью RSA приватного ключа,
 * - проверки подписи с помощью RSA публичного ключа.
 * 
 * @warning Для работы требуется библиотека OpenSSL.
 */
class CryptoUtils
{
public:
    /**
     * @brief Вычисляет SHA-256 хеш строки.
     * @param input Входные данные для хеширования.
     * @return HEX-строка с хешем (длиной 64 символа).
     * @throw std::runtime_error При ошибках в OpenSSL.
     */
    static std::string calculateHash(const std::string &input);

    /**
     * @brief Подписывает данные с использованием RSA приватного ключа.
     * @param data Данные для подписи.
     * @param privateKeyPEM Приватный ключ в формате PEM.
     * @return HEX-строка с подписью.
     * @throw std::runtime_error При ошибках:
     * - неверный формат ключа,
     * - ошибки OpenSSL.
     */
    static std::string signData(const std::string &data, 
                            const std::string &privateKeyPEM);


        /**
     * @brief Проверяет RSA подпись данных.
     * @param data Исходные данные.
     * @param signatureHex Подпись в HEX-формате.
     * @param publicKeyPEM Публичный ключ в формате PEM.
     * @return true - подпись верна, false - ошибка проверки.
     * @note Не бросает исключения, ошибки логируются в ConsoleUI.
     */
    static bool verifySignature(const std::string& data,
                            const std::string& signatureHex,
                            const std::string& publicKeyPEM);
};
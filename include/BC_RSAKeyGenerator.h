// BC_RSAKeyGenerator.h
#pragma once

// Системные библиотеки
#include <string>

// OpenSSL компоненты
#include <openssl/evp.h>


/**
 * @brief Класс для генерации RSA-ключей
 *
 * Класс RSAKeyGenerator предоставляет статические методы для генерации RSA-ключей
 * и их преобразования в текстовый формат PEM. Это позволяет использовать функциональность
 * без необходимости создания экземпляра класса.
 */
class RSAKeyGenerator
{
public:
    /**
     * Генерация пары RSA-ключей.
     * @param keyLength Длина ключа в битах (по умолчанию 2048).
     * @return Указатель на сгенерированный ключ (EVP_PKEY) или nullptr в случае ошибки.
     */
    static EVP_PKEY *generateRSAKeyPair(int keyLength = 2048);

    /**
     * Преобразование приватного ключа в строку в формате PEM.
     * @param pkey Указатель на приватный ключ (EVP_PKEY).
     * @return Строка в формате PEM или пустая строка в случае ошибки.
     */
    static std::string getPEMFromPrivateKey(EVP_PKEY *pkey);

    /**
     * Преобразование публичного ключа в строку в формате PEM.
     * @param pkey Указатель на публичный ключ (EVP_PKEY).
     * @return Строка в формате PEM или пустая строка в случае ошибки.
     */
    static std::string getPEMFromPublicKey(EVP_PKEY *pkey);
};
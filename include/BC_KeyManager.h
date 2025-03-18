// BC_KeyManager.h
#pragma once

// Системные библиотеки
#include <map>
#include <string>
#include <vector>

/**
 * @brief Класс для управления RSA-ключами пользователей.
 *
 * Класс KeyManager предназначен для управления RSA-ключами пользователей.
 * Он позволяет генерировать, сохранять и предоставлять доступ к публичным ключам
 * Приватный ключ сохраняется в формате PEM и удаляется из памяти.
 */
class KeyManager
{
private:
    std::map<std::string, std::string> publicKeys;  ///< Карта для хранения публичных ключей пользователей

    /**
     * Генерирует пару RSA-ключей для указанного пользователя.
     * @param username Имя пользователя, для которого генерируются ключи.
     */    
    void generateAndSaveKeys(const std::string &username);

public:
    /**
     * Конструктор класса KeyManager.
     * Генерирует и сохраняет ключи для каждого пользователя из списка.
     * @param users Список пользователей, для которых нужно сгенерировать ключи.
     */
    KeyManager(const std::vector<std::string> &users);

    /**
     * Добавляет ключи для нового пользователя.
     * @param username Имя пользователя.
     * @throws std::runtime_error Если пользователь уже существует.
     */
    void addUserKeys(const std::string &username);

    /**
     * Возвращает карту публичных ключей.
     * @return Ссылка на карту публичных ключей.
     */
    std::map<std::string, std::string> &getPublicKeys();

    /**
     * Укорачивает строку ключа, оставляя только начало и конец.
     * @param key Исходный ключ.
     * @param headLen Длина начала строки (по умолчанию 150 символов).
     * @param tailLen Длина конца строки (по умолчанию 150 символов).
     * @return Укороченная строка ключа.
     */
    std::string truncateKey(const std::string &key, int headLen = 150, int tailLen = 150);
};
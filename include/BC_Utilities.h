// BC_Utilities.h
#pragma once

// Системные библиотеки
#include <string>
#include <vector>

/**
 * @brief Класс для работы с консольным интерфейсом системы
 * 
 * Предоставляет методы для форматированного вывода информации:
 * - Заголовки и баннеры
 * - Цветовые стили для разных типов сообщений
 * - Элементы пользовательского интерфейса
 */
class ConsoleUI
{
public:
    /**
     * @brief Выводит главный баннер системы
     * 
     * Содержит информацию о версии, авторе и возможностях системы.
     * Использует псевдографику для визуального оформления.
     */
    static void printBanner();

    /**
     * @brief Выводит заголовок раздела с цветовым оформлением
     * @param title Текст заголовка
     */
    static void printHeader(const std::string &title);

    /**
     * @brief Выводит подзаголовок секции
     * @param title Текст подзаголовка в формате "=== Текст ==="
     */
    static void printSectionHeader(const std::string &title);

    /**
     * @brief Выводит стандартное сообщение
     * @param message Текст сообщения
     * @param newLine Добавлять перевод строки после сообщения
     */
    static void printDefault(const std::string &title, bool newLine = true);

    /**
     * @brief Выводит сообщение о процессе майнинга
     * @param message Текст сообщения с техническими деталями
     */
    static void printMining(const std::string &title);

    /**
     * @brief Выводит сообщение об успешной операции
     * @param message Текст подтверждения успеха
     */
    static void printSuccess(const std::string &message);

    /**
     * @brief Выводит сообщение об ошибке
     * @param message Текст описания ошибки
     */   
    static void printError(const std::string &message);

    /**
     * @brief Выводит предупреждение
     * @param message Текст предупреждения
     */
    static void printWarning(const std::string &message);

    /**
     * @brief Выводит информационное сообщение
     * @param message Текст информации
     * @param newLine Добавлять перевод строки после сообщения
     */
    static void printInfo(const std::string &message, bool newLine = true);

    /**
     * @brief Рисует горизонтальный разделитель
     * @param symbol Символ для заполнения линии
     * @param length Длина разделителя в символах
     */
    static void printDivider(char symbol = '-', int length = 60);

    /**
     * @brief Выводит пункты меню
     * @param options Список текстовых пунктов меню
     */
    static void printMenuOptions(const std::vector<std::string> &options);

    /**
     * @brief Отображает главное меню системы.
     * @param user Имя текущего пользователя.
     */
    static void printMenu(std::string &user);
};


/**
 * @brief Класс утилит для работы со временем
 */
class TimeUtils
{
public:
    /**
     * @brief Возвращает текущее время в формате строки
     * @return Строка времени в формате "ГГГГ-ММ-ДД ЧЧ:ММ:СС"
     * @note Реализация учитывает платформозависимые функции работы со временем
     */
    static std::string getCurrentTime();
};


/**
 * @brief Класс для валидации входных данных
 */
class Validator
{
public:
    /**
     * @brief Проверяет корректность формата блокчейн-адреса
     * @param address Адрес для проверки
     * @return true - адрес соответствует требованиям:
     * - Длина 3-20 символов
     * - Только буквы(без кириллицы), цифры и подчеркивание
     */
    static bool isAddressFormatValid(const std::string &address);
};


/**
 * @class PersistenceManager
 * @brief Класс для управления сохранением и шифрованием данных.
 *
 * Предоставляет статические методы для шифрования данных, которые могут быть использованы
 * для безопасного хранения информации.
 */
class PersistenceManager
{
public:
    /**
     * @brief Шифрует строку с использованием заданного ключа.
     *
     * Использует алгоритм AES-256-CBC для шифрования данных. Вектор инициализации (IV)
     * генерируется случайным образом и добавляется к результату.
     *
     * @param plaintext Исходная строка для шифрования.
     * @param key Ключ шифрования (должен быть длиной 32 байта для AES-256).
     * @return Зашифрованная строка, содержащая IV и зашифрованные данные.
     * @throws std::runtime_error В случае ошибки шифрования.
     */
    static std::string encryptString(const std::string &plaintext, const std::string &key);
};
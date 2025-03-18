// main.cpp

// Системные библиотеки
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>


// Пользовательские заголовочные файлы
#include "BC_Block.h"         // Определение блока блокчейна
#include "BC_Transaction.h"   // Определение транзакций
#include "BC_Controller.h"    // Управление блокчейном
#include "BC_KeyManager.h"    // Управление ключами пользователей
#include "BC_Utilities.h"     // Вспомогательные функции и утилиты


int main()
{
    ConsoleUI::printBanner();

    // Инициализация Genesis пользователя
    std::vector<std::string> users = {"Genesis_User"};
    std::string currentUser = "Genesis_User";

    ConsoleUI::printSectionHeader("System Initialization");
    ConsoleUI::printInfo("Logged in as: " + currentUser);

    // Инициализация ключей
    KeyManager keyManager(users);

    // Инициализация блокчейна
    ConsoleUI::printSectionHeader("Genesis Block Creation");
    ConsoleUI::printInfo("Mining genesis block...\n");
    BlockchainController controller(keyManager.getPublicKeys());
    ConsoleUI::printSuccess("Genesis block created successfully!");

    // Главный цикл
    bool running = true;
    while (running)
    {
        ConsoleUI::printMenu(currentUser);
        int choice;
        std::cin >> choice;
        switch (choice)
        {
        case 1:
        { // Регистрация пользователя
            ConsoleUI::printSectionHeader("User Registration");
            ConsoleUI::printDefault("Enter new username: ", false);
            std::string newUser;
            std::cin >> newUser;

            try
            {
                if (!Validator::isAddressFormatValid(newUser))
                {
                    ConsoleUI::printError("Invalid username format. Use alphanumeric characters and underscores (3-20 chars)");
                    break;
                }
                controller.registerUser(newUser);
                keyManager.addUserKeys(newUser);
                ConsoleUI::printSuccess("User '" + newUser + "' registered successfully");
            }
            catch (const std::exception &e)
            {
                ConsoleUI::printError("Registration failed: " + std::string(e.what()));
            }
            break;
        }

        case 2:
        { // Список пользователей
            ConsoleUI::printSectionHeader("Registered Users");
            const auto &menu_users = keyManager.getPublicKeys();
            if (menu_users.empty())
            {
                ConsoleUI::printWarning("No users registered yet");
                break;
            }
            for (const auto &[user, key] : menu_users)
            {
                ConsoleUI::printDefault(" - " + user + " (balance: " + std::to_string(controller.getUserBalance(user)) + ")");
            }
            break;
        }

        case 3:
        { // Смена пользователя
            ConsoleUI::printSectionHeader("User Login");
            ConsoleUI::printDefault("Enter username: ", false);
            std::string user;
            std::cin >> user;

            if (keyManager.getPublicKeys().count(user))
            {
                currentUser = user;
                double balance = controller.getUserBalance(currentUser);
                ConsoleUI::printSuccess("Logged in as: " + currentUser);
                ConsoleUI::printInfo("Current balance: " + std::to_string(balance));
            }
            else
            {
                ConsoleUI::printError("User '" + user + "' not found");
            }
            break;
        }

        case 4:
        { // Создание транзакции
            ConsoleUI::printSectionHeader("New Transaction");
            std::cin.ignore();

            // Ввод получателя
            ConsoleUI::printDefault("Recipient's username: ", false);
            std::string receiver;
            std::getline(std::cin, receiver);

            if (receiver.empty())
            {
                ConsoleUI::printError("Recipient cannot be empty");
                break;
            }

            // Ввод суммы
            ConsoleUI::printDefault("Amount to send: ", false);
            double amount;
            if (!(std::cin >> amount))
            {
                ConsoleUI::printError("Invalid amount format");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }

            // Проверка баланса
            double balance = controller.getUserBalance(currentUser);
            if (amount > balance)
            {
                ConsoleUI::printError("Insufficient funds. Available: " + std::to_string(balance));
                break;
            }

            // Загрузка приватного ключа
            std::cin.ignore();
            ConsoleUI::printInfo("Security Verification");
            ConsoleUI::printDefault("Path to private key file (" + currentUser + "_private.pem): ", false);
            std::string keyPath;
            std::getline(std::cin, keyPath);

            if (!keyPath.ends_with("_private.pem"))
            {
                ConsoleUI::printError("Invalid key file format");
                break;
            }

            std::ifstream keyFile(keyPath);
            if (!keyFile)
            {
                ConsoleUI::printError("Failed to open key file: " + keyPath);
                break;
            }

            std::string privateKey(
                (std::istreambuf_iterator<char>(keyFile)),
                std::istreambuf_iterator<char>());
            keyFile.close();

            if (privateKey.empty())
            {
                ConsoleUI::printError("Private key is empty");
                break;
            }

            // Создание и подпись транзакции
            try
            {
                ConsoleUI::printSectionHeader("Processing Transaction");
                Transaction tx(currentUser, receiver, amount);
                tx.signTransaction(privateKey);

                ConsoleUI::printInfo("Transaction Details:");
                ConsoleUI::printDefault(
                    " - Sender:    " + tx.getSender() + "\n" +
                    " - Receiver:  " + tx.getReceiver() + "\n" +
                    " - Amount:    " + std::to_string(tx.getAmount()) + "\n" +
                    " - TX ID:     " + tx.getTxId().substr(0, 12) + "...\n");

                controller.processTransactions({tx});
            }
            catch (const std::exception &e)
            {
                ConsoleUI::printError("Transaction failed: " + std::string(e.what()));
            }
            break;
        }

        case 5: // Просмотр блокчейна
            controller.printBlockchain();
            break;

        case 6:
        { // Сохранение блокчейна
            ConsoleUI::printSectionHeader("Blockchain Backup");
            std::string filename = "blockchain.dat";
            std::string encryptionKey = "mysecretkeymysecretkeymysecretkey!!";

            controller.saveBlockchain(filename, encryptionKey);
            ConsoleUI::printWarning("Keep encryption key safe: " + encryptionKey);
            break;
        }

        case 7:
        { // Валидация
            ConsoleUI::printSectionHeader("Blockchain Validation");
            if (controller.isBlockchainValid())
            {
                ConsoleUI::printSuccess("Blockchain integrity verified!");
            }
            else
            {
                ConsoleUI::printError("Blockchain validation failed!");
            }
            break;
        }

        case 8: // Выход
            running = false;
            ConsoleUI::printSectionHeader("System Shutdown");
            ConsoleUI::printInfo("Thank you for using Base Blockchain Transaction System!");
            break;

        default:
            ConsoleUI::printError("Invalid menu option");
            break;
        }
    }
    return 0;
}
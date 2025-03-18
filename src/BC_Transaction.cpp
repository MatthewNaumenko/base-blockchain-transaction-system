// BC_Transaction.cpp
#include "BC_Transaction.h"
#include "BC_CryptoUtils.h"
#include "BC_Utilities.h"

// Системные библиотеки (только для реализации)
#include <sstream>

// Реализация методов Transaction
Transaction::Transaction(const std::string &from,
                         const std::string &to,
                         double value,
                         const std::string &meta)
    : sender(Validator::isAddressFormatValid(from) ? from : ""),
      receiver(Validator::isAddressFormatValid(to) ? to : ""),
      amount(value > 0 ? value : 0),
      metadata(meta)
{
    // Генерация уникального идентификатора транзакции
    const std::string timePoint = TimeUtils::getCurrentTime();
    timestamp = timePoint;
    txId = CryptoUtils::calculateHash(
        sender + receiver + std::to_string(amount) + timePoint + meta);
}

std::string Transaction::getDataToSign() const
{
    // Формируем детерминированную строку для подписи
    return txId + sender + receiver +
           std::to_string(amount) + timestamp + metadata;
}

void Transaction::signTransaction(const std::string &privateKeyPEM)
{
    if (!signature.empty())
    {
        throw std::runtime_error("Transaction already signed");
    }

    // Верификация входных данных перед подписанием
    if (sender.empty() || receiver.empty() || amount <= 0)
    {
        throw std::runtime_error("Invalid transaction parameters");
    }

    std::string dataToSign = getDataToSign();
    ConsoleUI::printInfo(
        "Transaction signing initiated: " + txId +
        "\nData to sign: [" + dataToSign + "]\n");

    ConsoleUI::printInfo("Starting digital signature verification for transaction " + txId);

    // Попытка криптографической подписи данных
    signature = CryptoUtils::signData(dataToSign, privateKeyPEM);
}

std::string Transaction::toString() const
{
    std::stringstream ss;
    ss << "txId: " << getTxId() << ", ";
    ss << "From: " << getSender() << ", ";
    ss << "To: " << getReceiver() << ", ";
    ss << "Amount: " << std::fixed << getAmount() << " BTC\n"
       << ", ";
    ss << "Timestamp: " << getTimestamp() << ", ";
    ss << "Metadata: " << metadata << ", ";
    ss << "Signature: " << getSignature();
    return ss.str();
}

// Геттеры
const std::string &Transaction::getTxId() const { return txId; }
const std::string &Transaction::getSender() const { return sender; }
const std::string &Transaction::getSignature() const { return signature; }
const std::string &Transaction::getReceiver() const { return receiver; }
const std::string &Transaction::getTimestamp() const { return timestamp; }
const std::string &Transaction::getMetadata() const { return metadata; }
const double &Transaction::getAmount() const { return amount; }
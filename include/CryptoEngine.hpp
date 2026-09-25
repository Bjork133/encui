#pragma once

#include <string>

class CryptoEngine {
public:
    virtual ~CryptoEngine() = default;
    virtual std::string encrypt(const std::string& plaintext) = 0;
    virtual std::string decrypt(const std::string& ciphertext) = 0;
};

#pragma once

#include "CryptoEngine.hpp"
#include <string>

class CustomCipher : public CryptoEngine {
public:
    CustomCipher() = default;
    std::string encrypt(const std::string& plaintext) override {
        // TODO: implement custom encryption
        return plaintext; // identity for now
    }
    std::string decrypt(const std::string& ciphertext) override {
        // TODO: implement custom decryption
        return ciphertext; // identity for now
    }
};

#pragma once

#include "CryptoEngine.hpp"
#include <vector>
#include <string>

class ChaCha20 : public CryptoEngine {
public:
    ChaCha20(const std::vector<unsigned char>& key, const std::vector<unsigned char>& nonce);
    std::string encrypt(const std::string& plaintext) override;
    std::string decrypt(const std::string& ciphertext) override;
private:
    std::vector<unsigned char> key_;
    std::vector<unsigned char> nonce_;
};

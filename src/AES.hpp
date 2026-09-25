#pragma once

#include "CryptoEngine.hpp"
#include <vector>
#include <string>

class AES : public CryptoEngine {
public:
    AES(const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
    std::string encrypt(const std::string& plaintext) override;
    std::string decrypt(const std::string& ciphertext) override;
private:
    std::vector<unsigned char> key_;
    std::vector<unsigned char> iv_;
};

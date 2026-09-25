#pragma once

#include "CryptoEngine.hpp"
#include <memory>
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

class RSAEngine : public CryptoEngine {
public:
    RSAEngine(const std::string& publicKeyPem, const std::string& privateKeyPem);
    std::string encrypt(const std::string& plaintext) override;
    std::string decrypt(const std::string& ciphertext) override;
private:
    struct RSADeleter { void operator()(RSA* ptr) const { RSA_free(ptr); } };
    std::unique_ptr<RSA, RSADeleter> publicKey_;
    std::unique_ptr<RSA, RSADeleter> privateKey_;
};

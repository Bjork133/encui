#include "RSA.hpp"
#include "CryptoEngine.hpp"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <memory>
#include <vector>
#include <stdexcept>

RSAEngine::RSAEngine(const std::string& publicKeyPem, const std::string& privateKeyPem) {
    // Load public key
    BIO* pubBio = BIO_new_mem_buf(publicKeyPem.data(), static_cast<int>(publicKeyPem.size()));
    if (!pubBio) throw std::runtime_error("Failed to create BIO for public key");
    RSA* pub = PEM_read_bio_RSAPublicKey(pubBio, nullptr, nullptr, nullptr);
    BIO_free(pubBio);
    if (!pub) throw std::runtime_error("Failed to load RSA public key");
    publicKey_.reset(pub);

    // Load private key
    BIO* privBio = BIO_new_mem_buf(privateKeyPem.data(), static_cast<int>(privateKeyPem.size()));
    if (!privBio) throw std::runtime_error("Failed to create BIO for private key");
    RSA* priv = PEM_read_bio_RSAPrivateKey(privBio, nullptr, nullptr, nullptr);
    BIO_free(privBio);
    if (!priv) throw std::runtime_error("Failed to load RSA private key");
    privateKey_.reset(priv);
}

std::string RSAEngine::encrypt(const std::string& plaintext) {
    int keySize = RSA_size(publicKey_.get());
    std::vector<unsigned char> encrypted(keySize);
    int result = RSA_public_encrypt(
        static_cast<int>(plaintext.size()),
        reinterpret_cast<const unsigned char*>(plaintext.data()),
        encrypted.data(),
        publicKey_.get(), RSA_PKCS1_OAEP_PADDING);
    if (result == -1) {
        throw std::runtime_error("RSA encryption failed: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
    }
    return std::string(reinterpret_cast<char*>(encrypted.data()), result);
}

std::string RSAEngine::decrypt(const std::string& ciphertext) {
    int keySize = RSA_size(privateKey_.get());
    std::vector<unsigned char> decrypted(keySize);
    int result = RSA_private_decrypt(
        static_cast<int>(ciphertext.size()),
        reinterpret_cast<const unsigned char*>(ciphertext.data()),
        decrypted.data(),
        privateKey_.get(), RSA_PKCS1_OAEP_PADDING);
    if (result == -1) {
        throw std::runtime_error("RSA decryption failed: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
    }
    return std::string(reinterpret_cast<char*>(decrypted.data()), result);
}

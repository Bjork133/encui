#include "ChaCha20.hpp"
#include "CryptoEngine.hpp"
#include <sodium.h>
#include <stdexcept>
#include <vector>

ChaCha20::ChaCha20(const std::vector<unsigned char>& key, const std::vector<unsigned char>& nonce)
    : key_(key), nonce_(nonce) {
    if (key_.size() != crypto_aead_chacha20poly1305_ietf_KEYBYTES)
        throw std::runtime_error("ChaCha20 key must be 32 bytes");
    if (nonce_.size() != crypto_aead_chacha20poly1305_ietf_NPUBBYTES)
        throw std::runtime_error("ChaCha20 nonce must be 12 bytes");
}

std::string ChaCha20::encrypt(const std::string& plaintext) {
    unsigned long long ciphertext_len = 0;
    std::vector<unsigned char> ciphertext(plaintext.size() + crypto_aead_chacha20poly1305_ietf_ABYTES);
    if (crypto_aead_chacha20poly1305_ietf_encrypt(
            ciphertext.data(), &ciphertext_len,
            reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size(),
            nullptr, 0, nullptr, nonce_.data(), key_.data()) != 0) {
        throw std::runtime_error("ChaCha20 encryption failed");
    }
    ciphertext.resize(ciphertext_len);
    return std::string(reinterpret_cast<char*>(ciphertext.data()), ciphertext.size());
}

std::string ChaCha20::decrypt(const std::string& ciphertext) {
    unsigned long long plaintext_len = 0;
    std::vector<unsigned char> plaintext(ciphertext.size());
    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            plaintext.data(), &plaintext_len, nullptr,
            reinterpret_cast<const unsigned char*>(ciphertext.data()), ciphertext.size(),
            nullptr, 0, nonce_.data(), key_.data()) != 0) {
        throw std::runtime_error("ChaCha20 decryption failed");
    }
    plaintext.resize(plaintext_len);
    return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext.size());
}

#include "AES.hpp"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdexcept>
#include <vector>

static std::string getOpenSSLError() {
    char buf[256];
    ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
    return std::string(buf);
}

AES::AES(const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
    : key_(key), iv_(iv) {
    if (key_.size() != 32) throw std::runtime_error("AES key must be 32 bytes (256 bits)");
    if (iv_.size() != 16) throw std::runtime_error("AES IV must be 16 bytes (128 bits)");
}

std::string AES::encrypt(const std::string& plaintext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.data(), iv_.data())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_EncryptInit_ex failed: " + getOpenSSLError());
    }
    std::vector<unsigned char> outbuf(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int outlen = 0, tmplen = 0;
    if (1 != EVP_EncryptUpdate(ctx, outbuf.data(), &outlen,
                               reinterpret_cast<const unsigned char*>(plaintext.data()),
                               static_cast<int>(plaintext.size()))) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_EncryptUpdate failed: " + getOpenSSLError());
    }
    if (1 != EVP_EncryptFinal_ex(ctx, outbuf.data() + outlen, &tmplen)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_EncryptFinal_ex failed: " + getOpenSSLError());
    }
    outlen += tmplen;
    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(outbuf.data()), outlen);
}

std::string AES::decrypt(const std::string& ciphertext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.data(), iv_.data())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptInit_ex failed: " + getOpenSSLError());
    }
    std::vector<unsigned char> outbuf(ciphertext.size() + EVP_MAX_BLOCK_LENGTH);
    int outlen = 0, tmplen = 0;
    if (1 != EVP_DecryptUpdate(ctx, outbuf.data(), &outlen,
                               reinterpret_cast<const unsigned char*>(ciphertext.data()),
                               static_cast<int>(ciphertext.size()))) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptUpdate failed: " + getOpenSSLError());
    }
    if (1 != EVP_DecryptFinal_ex(ctx, outbuf.data() + outlen, &tmplen)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptFinal_ex failed: " + getOpenSSLError());
    }
    outlen += tmplen;
    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(outbuf.data()), outlen);
}

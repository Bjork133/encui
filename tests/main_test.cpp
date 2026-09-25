#include "gtest/gtest.h"
#include "CryptoEngine.hpp"
#include "AES.hpp"
#include "ChaCha20.hpp"
#include "RSA.hpp"

TEST(AES_Test, EncryptDecrypt) {
    std::vector<unsigned char> key(32, 0x01);
    std::vector<unsigned char> iv(16, 0x02);
    AES aes(key, iv);
    std::string plain = "Hello World";
    std::string cipher = aes.encrypt(plain);
    std::string recovered = aes.decrypt(cipher);
    EXPECT_EQ(plain, recovered);
}

TEST(ChaCha20_Test, EncryptDecrypt) {
    std::vector<unsigned char> key(crypto_aead_chacha20poly1305_ietf_KEYBYTES, 0x03);
    std::vector<unsigned char> nonce(crypto_aead_chacha20poly1305_ietf_NPUBBYTES, 0x04);
    ChaCha20 chacha(key, nonce);
    std::string plain = "TestMessage";
    std::string cipher = chacha.encrypt(plain);
    std::string recovered = chacha.decrypt(cipher);
    EXPECT_EQ(plain, recovered);
}

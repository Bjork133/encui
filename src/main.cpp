#include "CryptoEngine.hpp"
#include "AES.hpp"
#include "RSA.hpp"
#include "ChaCha20.hpp"
#include "CustomCipher.hpp"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <getopt.h>
#include <memory>

void print_usage() {
    std::cout << "Usage: encui -a|-r|-c|-x <mode> [options]\n"
                 "  -a  AES (requires key and iv)\n"
                 "  -r  RSA (requires pub/priv keys)\n"
                 "  -c  ChaCha20\n"
                 "  -x  custom\n"
                 "Options:\n"
                 "  -k <hex>   key (hex)\n"
                 "  -i <hex>   iv (hex) for AES\n"
                 "  -n <hex>   nonce (hex) for ChaCha20\n"
                 "  -p <file>  public key PEM file (RSA)\n"
                 "  -s <file>  private key PEM file (RSA)\n"
                 "  -m <msg>   message to encrypt/decrypt\n"
                 "  -d        decrypt mode (default encrypt)\n";
}

std::vector<unsigned char> hex_to_bytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteStr = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteStr, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

int main(int argc, char* argv[]) {
    bool decrypt = false;
    char mode = 0;
    std::string keyHex, ivHex, nonceHex, pubPath, privPath, message;

    int opt;
    while ((opt = getopt(argc, argv, "arcxk:i:n:p:s:m:d")) != -1) {
        switch (opt) {
            case 'a': mode = 'a'; break;
            case 'r': mode = 'r'; break;
            case 'c': mode = 'c'; break;
            case 'x': mode = 'x'; break;
            case 'k': keyHex = optarg; break;
            case 'i': ivHex = optarg; break;
            case 'n': nonceHex = optarg; break;
            case 'p': pubPath = optarg; break;
            case 's': privPath = optarg; break;
            case 'm': message = optarg; break;
            case 'd': decrypt = true; break;
            default: print_usage(); return 1;
        }
    }

    if (!mode || message.empty()) {
        print_usage();
        return 1;
    }

    std::unique_ptr<CryptoEngine> engine;
    try {
        switch (mode) {
            case 'a': {
                if (keyHex.empty() || ivHex.empty()) { std::cerr << "Key and IV required for AES\n"; return 1; }
                engine = std::make_unique<AES>(hex_to_bytes(keyHex), hex_to_bytes(ivHex));
                break;
            }
            case 'r': {
                if (pubPath.empty() || privPath.empty()) { std::cerr << "Public and private key files required for RSA\n"; return 1; }
                std::ifstream pubFile(pubPath); std::string pubPem((std::istreambuf_iterator<char>(pubFile)), std::istreambuf_iterator<char>());
                std::ifstream privFile(privPath); std::string privPem((std::istreambuf_iterator<char>(privFile)), std::istreambuf_iterator<char>());
                engine = std::make_unique<RSAEngine>(pubPem, privPem);
                break;
            }
            case 'c': {
                if (keyHex.empty() || nonceHex.empty()) { std::cerr << "Key and nonce required for ChaCha20\n"; return 1; }
                engine = std::make_unique<ChaCha20>(hex_to_bytes(keyHex), hex_to_bytes(nonceHex));
                break;
            }
            case 'x': {
                engine = std::make_unique<CustomCipher>();
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error initializing engine: " << e.what() << "\n";
        return 1;
    }

    try {
        if (decrypt) {
            std::cout << engine->decrypt(message) << "\n";
        } else {
            std::cout << engine->encrypt(message) << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Operation failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

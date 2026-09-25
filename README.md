# EncEncryptor

## Overview

`encui` — терминальный инструмент для шифрования и расшифровки сообщений. Поддерживает:

- AES‑256‑CBC (OpenSSL)
- RSA (OpenSSL, PEM‑ключи)
- ChaCha20‑Poly1305 (libsodium)
- пользовательский алгоритм (заглушка)

Работает в режиме командной строки, принимает параметры для ключей/IV/nonce, сообщения и режимов шифрования/расшифровки.

## Сборка

```bash
mkdir build && cd build
cmake ..
make
```

## Использование

```bash
./encui -a -k <hex_key> -i <hex_iv> -m "Hello"          # AES‑encrypt
./encui -a -d -k <hex_key> -i <hex_iv> -m "<cipher>"   # AES‑decrypt
# similarly for -r (RSA), -c (ChaCha20), -x (custom)
```

## Пакет AUR

PKGBUILD будет создан в `PKGBUILD` корневой директории.

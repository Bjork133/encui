use std::env;
use std::process::exit;

use conway_aes::{encrypt as conway_encrypt, decrypt as conway_decrypt, ConwayParams};

fn hex_encode(data: &[u8]) -> String {
    data.iter().map(|b| format!("{:02x}", b)).collect()
}

fn hex_decode(s: &str) -> Result<Vec<u8>, String> {
    if s.len() % 2 != 0 {
        return Err("hex length must be even".into());
    }
    let mut out = Vec::with_capacity(s.len() / 2);
    let bytes = s.as_bytes();
    for i in (0..bytes.len()).step_by(2) {
        let hi = (bytes[i] as char).to_digit(16).ok_or("invalid hex")? as u8;
        let lo = (bytes[i + 1] as char).to_digit(16).ok_or("invalid hex")? as u8;
        out.push((hi << 4) | lo);
    }
    Ok(out)
}

fn caesar(data: &[u8], shift: u8, encrypt: bool) -> Vec<u8> {
    let s = if encrypt { shift % 26 } else { (26 - (shift % 26)) % 26 };
    data.iter()
        .map(|&b| {
            if b.is_ascii_lowercase() {
                b'a' + (b - b'a' + s) % 26
            } else if b.is_ascii_uppercase() {
                b'A' + (b - b'A' + s) % 26
            } else {
                b
            }
        })
        .collect()
}

fn vigenere(data: &[u8], key: &[u8], encrypt: bool) -> Result<Vec<u8>, String> {
    if key.is_empty() {
        return Err("vigenere key cannot be empty".into());
    }
    let key_bytes: Vec<u8> = key
        .iter()
        .filter(|b| b.is_ascii_alphabetic())
        .map(|b| b.to_ascii_lowercase() - b'a')
        .collect();
    if key_bytes.is_empty() {
        return Err("vigenere key must contain letters".into());
    }
    let mut ki = 0;
    let mut out = Vec::with_capacity(data.len());
    for &b in data {
        if b.is_ascii_lowercase() {
            let k = key_bytes[ki % key_bytes.len()];
            let s = if encrypt { k } else { (26 - k) % 26 };
            out.push(b'a' + (b - b'a' + s) % 26);
            ki += 1;
        } else if b.is_ascii_uppercase() {
            let k = key_bytes[ki % key_bytes.len()];
            let s = if encrypt { k } else { (26 - k) % 26 };
            out.push(b'A' + (b - b'A' + s) % 26);
            ki += 1;
        } else {
            out.push(b);
        }
    }
    Ok(out)
}

fn vernam(data: &[u8], key: &[u8]) -> Result<Vec<u8>, String> {
    if key.is_empty() {
        return Err("vernam key cannot be empty".into());
    }
    if key.len() < data.len() {
        return Err("vernam key must be at least as long as data".into());
    }
    Ok(data.iter().zip(key.iter()).map(|(&d, &k)| d ^ k).collect())
}

fn aes_gcm_run(data: &[u8], key_hex: &str, nonce_hex: &str, encrypt: bool) -> Result<Vec<u8>, String> {
    use aes_gcm::{Aes256Gcm, KeyInit, aead::{Aead, generic_array::GenericArray}};
    let key = hex_decode(key_hex)?;
    let nonce = hex_decode(nonce_hex)?;
    if key.len() != 32 {
        return Err("aes-gcm key must be 32 bytes (64 hex chars)".into());
    }
    if nonce.len() != 12 {
        return Err("aes-gcm nonce must be 12 bytes (24 hex chars)".into());
    }
    let cipher = Aes256Gcm::new(GenericArray::from_slice(&key));
    let n = GenericArray::from_slice(&nonce);
    if encrypt {
        cipher.encrypt(n, data).map_err(|e| format!("aes-gcm encrypt: {:?}", e))
    } else {
        cipher.decrypt(n, data).map_err(|e| format!("aes-gcm decrypt: {:?}", e))
    }
}

fn chacha_run(data: &[u8], key_hex: &str, nonce_hex: &str, encrypt: bool) -> Result<Vec<u8>, String> {
    use chacha20poly1305::{ChaCha20Poly1305, KeyInit, aead::{Aead, generic_array::GenericArray}};
    let key = hex_decode(key_hex)?;
    let nonce = hex_decode(nonce_hex)?;
    if key.len() != 32 {
        return Err("chacha key must be 32 bytes (64 hex chars)".into());
    }
    if nonce.len() != 12 {
        return Err("chacha nonce must be 12 bytes (24 hex chars)".into());
    }
    let cipher = ChaCha20Poly1305::new(GenericArray::from_slice(&key));
    let n = GenericArray::from_slice(&nonce);
    if encrypt {
        cipher.encrypt(n, data).map_err(|e| format!("chacha encrypt: {:?}", e))
    } else {
        cipher.decrypt(n, data).map_err(|e| format!("chacha decrypt: {:?}", e))
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut algo = String::from("conway-aes");
    let mut mode = String::from("encrypt");
    let mut data = String::new();
    let mut public = String::new();
    let mut private = String::new();
    let mut method = String::new();
    let mut shift = String::new();
    let mut key = String::new();
    let mut nonce = String::new();
    let mut data_hex = false;

    let mut i = 1;
    while i < args.len() {
        match args[i].as_str() {
            "--algo" => { i += 1; algo = args[i].clone(); }
            "--mode" => { i += 1; mode = args[i].clone(); }
            "--data" => { i += 1; data = args[i].clone(); }
            "--data-hex" => { data_hex = true; }
            "--public" => { i += 1; public = args[i].clone(); }
            "--private" => { i += 1; private = args[i].clone(); }
            "--method" => { i += 1; method = args[i].clone(); }
            "--shift" => { i += 1; shift = args[i].clone(); }
            "--key" => { i += 1; key = args[i].clone(); }
            "--nonce" => { i += 1; nonce = args[i].clone(); }
            _ => {}
        }
        i += 1;
    }

    let encrypt = mode != "decrypt";
    let input = if data_hex {
        match hex_decode(&data) {
            Ok(v) => v,
            Err(e) => { eprintln!("Error: {}", e); exit(1); }
        }
    } else {
        data.into_bytes()
    };

    let result: Result<Vec<u8>, String> = match algo.as_str() {
        "conway-aes" => {
            let params = ConwayParams {
                public: public.into_bytes(),
                private: private.into_bytes(),
                method: method.into_bytes(),
            };
            let r = if encrypt {
                conway_encrypt(&input, &params)
            } else {
                conway_decrypt(&input, &params)
            };
            r.map_err(|e| format!("{}", e))
        }
        "caesar" => {
        let s: u8 = match shift.parse() {
                Ok(v) => v,
                Err(_) => { eprintln!("Error: shift must be 0..255"); exit(1); }
            };
            Ok(caesar(&input, s, encrypt))
        }
        "vigenere" => vigenere(&input, key.as_bytes(), encrypt),
        "vernam" => vernam(&input, key.as_bytes()),
        "aes-gcm" => aes_gcm_run(&input, &key, &nonce, encrypt),
        "chacha20poly1305" => chacha_run(&input, &key, &nonce, encrypt),
        other => Err(format!("unknown algo: {}", other)),
    };

    match result {
        Ok(out) => println!("{}", hex_encode(&out)),
        Err(e) => { eprintln!("Error: {}", e); exit(1); }
    }
}

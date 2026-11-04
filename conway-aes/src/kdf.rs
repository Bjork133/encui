use sha2::{Digest, Sha256};

pub fn derive_aes_key(public: &[u8], private: &[u8]) -> [u8; 32] {
    let mut hasher = Sha256::new();
    hasher.update(b"conway-aes/key/v1");
    hasher.update(public);
    hasher.update(private);
    let result = hasher.finalize();
    let mut key = [0u8; 32];
    key.copy_from_slice(&result);
    key
}

pub fn derive_nonce(public: &[u8], private: &[u8]) -> [u8; 16] {
    let mut hasher = Sha256::new();
    hasher.update(b"conway-aes/nonce/v1");
    hasher.update(public);
    hasher.update(private);
    let result = hasher.finalize();
    let mut nonce = [0u8; 16];
    nonce.copy_from_slice(&result[..16]);
    nonce
}

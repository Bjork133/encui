use aes::Aes256;
use ctr::Ctr128BE;
use ctr::cipher::{KeyIvInit, StreamCipher};

type Aes256Ctr = Ctr128BE<Aes256>;

pub fn aes_ctr(data: &[u8], key: &[u8; 32], nonce: &[u8; 16]) -> Vec<u8> {
    let mut cipher = Aes256Ctr::new(key.into(), nonce.into());
    let mut buf = data.to_vec();
    cipher.apply_keystream(&mut buf);
    buf
}

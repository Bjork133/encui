mod params;
mod error;
mod kdf;
mod method;
mod life;
mod aes_layer;

pub use params::ConwayParams;
pub use error::ConwayError;

use kdf::{derive_aes_key, derive_nonce};
use method::{xor_with_method, derive_generations};
use life::apply_n_generations;
use aes_layer::aes_ctr;

pub fn encrypt(data: &[u8], params: &ConwayParams) -> Result<Vec<u8>, ConwayError> {
    if params.method.is_empty() {
        return Err(ConwayError::EmptyMethod);
    }
    let xored = xor_with_method(data, &params.method);
    let n = derive_generations(&params.method);
    let after_life = apply_n_generations(&xored, n);
    let key = derive_aes_key(&params.public, &params.private);
    let nonce = derive_nonce(&params.public, &params.private);
    Ok(aes_ctr(&after_life, &key, &nonce))
}

pub fn decrypt(data: &[u8], params: &ConwayParams) -> Result<Vec<u8>, ConwayError> {
    if params.method.is_empty() {
        return Err(ConwayError::EmptyMethod);
    }
    let key = derive_aes_key(&params.public, &params.private);
    let nonce = derive_nonce(&params.public, &params.private);
    let after_aes = aes_ctr(data, &key, &nonce);
    let n = derive_generations(&params.method);
    let after_life = apply_n_generations(&after_aes, n);
    Ok(xor_with_method(&after_life, &params.method))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn roundtrip() {
        let params = ConwayParams {
            public: b"pub".to_vec(),
            private: b"priv".to_vec(),
            method: b"my_method_key_123".to_vec(),
        };
        let msg = b"Hello, Conway-AES!";
        let ct = encrypt(msg, &params).unwrap();
        let pt = decrypt(&ct, &params).unwrap();
        assert_eq!(msg.as_slice(), pt.as_slice());
    }

    #[test]
    fn roundtrip_various_lengths() {
        let params = ConwayParams {
            public: vec![1, 2, 3],
            private: vec![4, 5, 6, 7, 8],
            method: vec![9, 10, 11, 12],
        };
        for len in 0..200 {
            let msg: Vec<u8> = (0..len).map(|i| (i % 251) as u8).collect();
            let ct = encrypt(&msg, &params).unwrap();
            assert_eq!(ct.len(), msg.len());
            let pt = decrypt(&ct, &params).unwrap();
            assert_eq!(msg, pt);
        }
    }
}

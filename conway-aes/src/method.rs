pub fn xor_with_method(data: &[u8], method: &[u8]) -> Vec<u8> {
    if method.is_empty() {
        return data.to_vec();
    }
    data.iter()
        .zip(method.iter().cycle())
        .map(|(&d, &m)| d ^ m)
        .collect()
}

pub fn derive_generations(method: &[u8]) -> usize {
    if method.is_empty() {
        return 1;
    }
    let half_len = (method.len() / 2).max(1);
    // big-endian: первый байт — старший
    let mut value: u128 = 0;
    for &b in &method[..half_len] {
        value = (value << 8) | (b as u128);
    }
    let mut n = value as usize;
    while n >= 100 {
        n /= 3;
    }
    n.max(1)
}

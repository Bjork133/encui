pub fn apply_n_generations(data: &[u8], n: usize) -> Vec<u8> {
    if data.is_empty() || n == 0 {
        return data.to_vec();
    }
    let len = data.len();
    let k = (len as f64).sqrt().ceil() as usize;
    let total = k * k;
    let mut grid = vec![0u8; total];
    grid[..len].copy_from_slice(data);

    for _ in 0..n {
        // Обратимый проход: для каждой пары (2i, 2i+1) делаем
        // a' = a XOR b, b' = b (или a' = a, b' = b XOR a — сеть Фейстеля).
        // Это биекция на любой длине, границы не важны.
        let mut i = 0;
        while i + 1 < total {
            grid[i] ^= grid[i + 1];
            i += 2;
        }
        // Если total нечётно, последний байт не трогаем —
        // но тогда биекция сохраняется (он просто не меняется).
    }

    grid[..len].to_vec()
}

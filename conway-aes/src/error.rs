#[derive(Debug)]
pub enum ConwayError {
    EmptyMethod,
    AesError(String),
}

impl std::fmt::Display for ConwayError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ConwayError::EmptyMethod => write!(f, "method key cannot be empty"),
            ConwayError::AesError(s) => write!(f, "AES error: {}", s),
        }
    }
}

impl std::error::Error for ConwayError {}

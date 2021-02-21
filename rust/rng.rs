pub struct Rng {
    x: u64,
}

impl Rng {
    pub fn new() -> Self {
        Self {
            x: 88172645463325252,
        }
    }
    pub fn next(&mut self) -> u64 {
        self.x = self.x ^ (self.x.wrapping_shl(7));
        self.x = self.x ^ (self.x.wrapping_shr(9));
        self.x
    }
}

const MOD3: i32 = 3;

#[derive(Clone, Copy)]
pub struct ModInt3(pub i32);

impl std::fmt::Display for ModInt3 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.0)
    }
}

impl std::fmt::Debug for ModInt3 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.0)
    }
}

impl From<i32> for ModInt3 {
    fn from(item: i32) -> Self {
        Self(item % MOD3)
    }
}

fn mod_sub(mut v: i32) -> i32 {
    if v >= MOD3 {
        v -= MOD3
    }
    assert!(v >= 0);
    v
}

// void extgcd(BigInteger a, BigInteger b) {
//     BigInteger[] u = new BigInteger[] { a, BigInteger.ONE, BigInteger.ZERO };
//     BigInteger[] v = new BigInteger[] { b, BigInteger.ZERO, BigInteger.ONE };
//     while (!v[0].equals(BigInteger.ZERO)) {
//       BigInteger t = u[0].divide(v[0]);
//       for (int i = 0; i < 3; i++) {
//         u[i] = u[i].subtract(t.multiply(v[i]));
//       }
//       BigInteger[] tmp = u;
//       u   = v;
//       v = tmp;
//     }
//     // if (target.mod(u[0]).signum() == 0) {
//     //   System.err.println("find2 : " + a + "*" + u[1] + "+" + b + "*" + u[2] + "=" + target);
//     //
//     //
//   }

// static long modInverse(final long n, final long m) {
//     final long[] res = extGcd(n, m);
//     return res[2] != 1 ? -1 : (m + res[0]) % m;
// }

// // ax+by=c(=gcd(a,b))
// // �Ȃ�x,y�����߂�
// // return [x,y,c]
// static long[] extGcd(final long a, final long b) {
//     if (b == 0) {
//         return new long[] { 1, 0, a };
//     }
//     final long[] res = extGcd(b, a % b);
//     long y = res[0];
//     final long x = res[1];
//     y -= (a / b) * x;
//     res[0] = x;
//     res[1] = y;
//     return res;
// }

// a * r.0 + b * r.1 = r.2
pub fn ext_gcd(a: i32, b: i32) -> (i32, i32, i32) {
    let mut u = [a, 1, 0];
    let mut v = [b, 0, 1];
    while v[0] != 0 {
        let t = u[0] / v[0];
        (0..3).for_each(|i| u[i] -= t * v[i]);
        std::mem::swap(&mut u, &mut v);
    }
    (u[1], u[2], u[0])
}

#[test]
fn test_ext_gcd() {
    let g = ext_gcd(10, 3);
    assert_eq!(10 * g.0 + 3 * g.1, g.2);
    assert_eq!(g, (1, -3, 1));
}

fn mod_inv(v: i32) -> i32 {
    let mut g = ext_gcd(v, MOD3);
    assert_eq!(g.2, 1);
    if g.0 < 0 {
        g.0 += MOD3;
    }
    g.0
}

impl ModInt3 {
    pub fn us(&self) -> usize {
        self.0 as usize
    }
}

impl std::ops::AddAssign for ModInt3 {
    fn add_assign(&mut self, rhs: Self) {
        self.0 = mod_sub(self.0 + rhs.0)
    }
}

impl std::ops::Add for ModInt3 {
    type Output = ModInt3;

    fn add(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        let mut v = self;
        v += rhs;
        // ModInt3(mod_sub(self.0 + rhs.0))
        v
    }
}

impl std::ops::Add for &ModInt3 {
    type Output = ModInt3;

    fn add(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        ModInt3(mod_sub(self.0 + rhs.0))
    }
}

impl std::ops::Sub for ModInt3 {
    type Output = ModInt3;

    fn sub(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        ModInt3(mod_sub(self.0 + MOD3 - rhs.0))
    }
}

impl std::ops::Sub for &ModInt3 {
    type Output = ModInt3;

    fn sub(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        ModInt3(mod_sub(self.0 + MOD3 - rhs.0))
    }
}

impl std::ops::Mul for ModInt3 {
    type Output = ModInt3;

    fn mul(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        ModInt3(self.0 * rhs.0 % MOD3)
    }
}

impl std::ops::Mul for &ModInt3 {
    type Output = ModInt3;

    fn mul(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        ModInt3(self.0 * rhs.0 % MOD3)
    }
}

impl std::ops::Div for ModInt3 {
    type Output = ModInt3;

    fn div(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        ModInt3(self.0 * mod_inv(rhs.0) % MOD3)
    }
}

impl std::ops::Div for &ModInt3 {
    type Output = ModInt3;

    fn div(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        ModInt3(self.0 * mod_inv(rhs.0) % MOD3)
    }
}

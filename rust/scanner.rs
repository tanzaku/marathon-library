use std::fmt::Debug;
use std::io::BufRead;
use std::io::StdinLock;
use std::str::FromStr;

pub struct Scanner<'a> {
    buf_read: Box<dyn BufRead + 'a>,
    tokens: Vec<String>,
}

impl<'a> Scanner<'a> {
    pub fn new(buf_read: StdinLock<'a>) -> Self {
        Self {
            buf_read: Box::new(buf_read),
            tokens: Vec::new(),
        }
    }

    pub fn next<T: FromStr>(&mut self) -> T
    where
        T::Err: Debug,
    {
        while self.tokens.is_empty() {
            let mut s = String::new();
            if self.buf_read.read_line(&mut s).is_err() {
                panic!()
            }
            self.tokens = s
                .split_whitespace()
                .filter(|s| !s.is_empty())
                .rev()
                .map(str::to_string)
                .collect();
        }
        self.tokens.pop().unwrap().parse().unwrap()
    }
}

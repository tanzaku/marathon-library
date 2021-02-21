use super::mod_int::ModInt3;
use super::rng::Rng;

#[derive(Eq, PartialEq, Clone, Copy, Default, PartialOrd, Ord)]
pub struct P {
    pub x: i32,
    pub y: i32,
}

#[derive(Clone, Hash, Eq, PartialEq, Copy, Default, Debug)]
pub struct Action(pub i32);

#[derive(Clone, PartialEq, Eq)]
pub struct Gold {
    pub p: P,
    pub quantity: i32,
}

#[derive(Clone)]
pub struct State {
    pub player_index: i32,
    pub n: i32,
    pub step: usize,
    pub max_step: usize,
    pub hole: Vec<P>,
    pub public_gold: Vec<Gold>,
    pub private_gold: Vec<Gold>,
    pub agent: [P; 4],
    pub try_plan: [Action; 4],
    pub actual_plan: [Action; 4],
    pub score: [i32; 2],
    pub rest_total_gold: i32,
    pub rest_think_time: i32,
    pub histories: Vec<[Action; 4]>,
}

pub trait Plan {
    fn decide(&mut self, state: &State) -> Action;
}

const SAMURAI_MOVES: [P; 4] = [
    P { x: 0, y: 1 },
    P { x: -1, y: 0 },
    P { x: 0, y: -1 },
    P { x: 1, y: 0 },
];

const DOG_MOVES: [P; 8] = [
    P { x: 0, y: 1 },
    P { x: -1, y: 1 },
    P { x: -1, y: 0 },
    P { x: -1, y: -1 },
    P { x: 0, y: -1 },
    P { x: 1, y: -1 },
    P { x: 1, y: 0 },
    P { x: 1, y: 1 },
];

pub fn count(ps: &[P], elem: &P) -> usize {
    ps.iter().filter(|p| p == &elem).count()
}

impl P {
    pub fn manhatten_dist(&self, t: &P) -> i32 {
        let dx = self.x - t.x;
        let dy = self.y - t.y;
        dx.abs() + dy.abs()
    }
    pub fn l_inf_norm(&self, t: &P) -> i32 {
        let dx = self.x - t.x;
        let dy = self.y - t.y;
        std::cmp::max(dx.abs(), dy.abs())
    }
    pub fn dist2(&self, t: &P) -> i32 {
        let dx = self.x - t.x;
        let dy = self.y - t.y;
        dx * dx + dy * dy
    }
}

impl std::fmt::Display for P {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "({},{})", self.x, self.y)
    }
}

impl std::fmt::Debug for P {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "({},{})", self.x, self.y)
    }
}

impl std::ops::Add for P {
    type Output = P;

    fn add(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        P {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
        }
    }
}

impl std::ops::Add for &P {
    type Output = P;

    fn add(self, rhs: Self) -> <Self as std::ops::Add<Self>>::Output {
        P {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
        }
    }
}

impl std::ops::Sub for P {
    type Output = P;

    fn sub(self, rhs: Self) -> <Self as std::ops::Sub<Self>>::Output {
        P {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
        }
    }
}

impl std::ops::Sub for &P {
    type Output = P;

    fn sub(self, rhs: Self) -> <Self as std::ops::Sub<Self>>::Output {
        P {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
        }
    }
}

impl std::fmt::Display for Action {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.0)
    }
}

impl From<i32> for Action {
    fn from(v: i32) -> Action {
        Action(v)
    }
}

impl Action {
    pub fn samurai_actions() -> Vec<Action> {
        (-1..24)
            .filter(|a| *a < 0 || *a % 2 == 0)
            .map(Action::from)
            .collect()
    }

    pub fn dog_actions() -> Vec<Action> {
        (-1..8).map(Action::from).collect()
    }

    pub fn target_pos(&self, pos: &P) -> P {
        if self.0 < 0 {
            pos.clone()
        } else {
            pos + &DOG_MOVES[self.0 as usize % 8]
        }
    }

    fn move_pos(&self, pos: &P) -> P {
        if self.is_dig_or_fill() {
            pos.clone()
        } else {
            self.target_pos(pos)
        }
    }

    pub fn is_dig_or_fill(&self) -> bool {
        self.is_dig() || self.is_fill()
    }

    pub fn is_dig(&self) -> bool {
        self.0 >= 8 && self.0 < 16
    }

    pub fn is_fill(&self) -> bool {
        self.0 >= 16 && self.0 < 24
    }

    pub fn to_dig(&self) -> Self {
        assert!(self.0 != -1);
        Self(self.0 % 8 + 8)
    }

    pub fn to_fill(&self) -> Self {
        assert!(self.0 != -1);
        Self(self.0 % 8 + 16)
    }

    pub fn to_move(&self) -> Self {
        assert!(self.0 != -1);
        Self(self.0 % 8)
    }
}

fn samurai_move_positions(p: &P) -> Vec<P> {
    SAMURAI_MOVES.iter().map(|q| p + q).collect()
}

fn dog_move_positions(p: &P) -> Vec<P> {
    DOG_MOVES.iter().map(|q| p + q).collect()
}

fn to_move_action(p: &P, q: &P) -> Action {
    let d = q - p;
    DOG_MOVES
        .iter()
        .enumerate()
        .filter(|(_, v)| v == &&d)
        .next()
        .map(|(i, _)| Action::from(i as i32))
        .unwrap()
}

impl State {
    fn is_hole(&self, p: &P) -> bool {
        count(&self.hole, p) > 0
    }

    fn is_gold(&self, p: &P) -> bool {
        for g in &self.public_gold {
            if &g.p == p {
                return true;
            }
        }
        for g in &self.private_gold {
            if &g.p == p {
                return true;
            }
        }
        false
    }

    fn is_other_player_pos(&self, p: &P) -> bool {
        for i in 0..self.agent.len() {
            if i as i32 != self.player_index && &self.agent[i] == p {
                return true;
            }
        }
        false
    }

    // fn is_enemy_player_pos(&self, p: &P) -> bool {
    //     for i in 0..self.agent.len() {
    //         if i as i32 % 2 != self.player_index % 2 && &self.agent[i] == p {
    //             return true;
    //         }
    //     }
    //     false
    // }

    // pub fn calc_dist_by_samurai_without_enemy_agent(
    //     &self,
    //     s: &P,
    //     rng: &mut Rng,
    // ) -> (Vec<Vec<i32>>, Vec<Vec<Action>>) {
    //     let gen_next_move = samurai_move_positions;

    //     let n = self.n as usize;
    //     let mut dist = vec![vec![std::i32::MAX; n]; n];
    //     let mut prev = vec![vec![Action(-1); n]; n];
    //     let mut ques = vec![Vec::new(); 3];
    //     let mut que_idx = vec![0; 3];

    //     let mut qi: ModInt3 = 0.into();
    //     ques[0].push((s.clone(), 0));
    //     dist[s.y as usize][s.x as usize] = 0;
    //     let mut rest_que_size = 1;
    //     while rest_que_size > 0 {
    //         let qi_us = qi.us();
    //         while que_idx[qi_us] < ques[qi_us].len() {
    //             let p = ques[qi_us][que_idx[qi_us]].0;
    //             let d = ques[qi_us][que_idx[qi_us]].1;
    //             que_idx[qi_us] += 1;
    //             rest_que_size -= 1;
    //             gen_next_move(&p)
    //                 .into_iter()
    //                 .filter(|q| self.is_valid_pos(&q))
    //                 .filter(|q| !self.is_enemy_player_pos(&q)) // 他のエージェントの情報を考慮しない
    //                 .for_each(|q| {
    //                     let mut d = d + 1;
    //                     let mut prev_action = to_move_action(&p, &q);
    //                     if self.is_hole(&q) {
    //                         d += 1;
    //                         prev_action = prev_action.to_fill();
    //                     }
    //                     if self.is_gold(&q) {
    //                         prev_action = prev_action.to_dig();
    //                     }
    //                     if dist[p.y as usize][p.x as usize] != 0 {
    //                         prev_action = prev[p.y as usize][p.x as usize];
    //                     }
    //                     // eprintln!("check {} {}", dist[q.y as usize][q.x as usize], d);
    //                     if dist[q.y as usize][q.x as usize] > d
    //                         || dist[q.y as usize][q.x as usize] == d && rng.next() % 2 == 0
    //                     {
    //                         dist[q.y as usize][q.x as usize] = d;
    //                         prev[q.y as usize][q.x as usize] = prev_action;
    //                         ques[(qi + d.into()).us()].push((q, d));
    //                         rest_que_size += 1;
    //                     }
    //                 });
    //         }
    //         qi += 1.into();
    //     }
    //     (dist, prev)
    // }

    pub fn calc_dist_by_samurai_without_other_agent(
        &self,
        s: &P,
        rng: &mut Rng,
    ) -> (Vec<Vec<i32>>, Vec<Vec<Action>>) {
        let gen_next_move = samurai_move_positions;

        let n = self.n as usize;
        let mut dist = vec![vec![std::i32::MAX; n]; n];
        let mut prev = vec![vec![Action(-1); n]; n];
        let mut ques = vec![Vec::new(); 3];
        let mut que_idx = vec![0; 3];

        let mut qi: ModInt3 = 0.into();
        ques[0].push((s.clone(), 0));
        dist[s.y as usize][s.x as usize] = 0;
        let mut rest_que_size = 1;
        while rest_que_size > 0 {
            let qi_us = qi.us();
            while que_idx[qi_us] < ques[qi_us].len() {
                let p = ques[qi_us][que_idx[qi_us]].0;
                let d = ques[qi_us][que_idx[qi_us]].1;
                que_idx[qi_us] += 1;
                rest_que_size -= 1;
                gen_next_move(&p)
                    .into_iter()
                    .filter(|q| self.is_valid_pos(&q))
                    .for_each(|q| {
                        let mut d = d + 1;
                        let mut prev_action = to_move_action(&p, &q);
                        if self.is_hole(&q) {
                            d += 1;
                            prev_action = prev_action.to_fill();
                        }
                        if self.is_gold(&q) {
                            prev_action = prev_action.to_dig();
                        }
                        if dist[p.y as usize][p.x as usize] != 0 {
                            prev_action = prev[p.y as usize][p.x as usize];
                        }
                        // eprintln!("check {} {}", dist[q.y as usize][q.x as usize], d);
                        if dist[q.y as usize][q.x as usize] > d
                            || dist[q.y as usize][q.x as usize] == d && rng.next() % 2 == 0
                        {
                            dist[q.y as usize][q.x as usize] = d;
                            prev[q.y as usize][q.x as usize] = prev_action;
                            ques[(qi + d.into()).us()].push((q, d));
                            rest_que_size += 1;
                        }
                    });
            }
            qi += 1.into();
        }
        (dist, prev)
    }

    pub fn calc_dist_by_samurai(&self, s: &P, rng: &mut Rng) -> (Vec<Vec<i32>>, Vec<Vec<Action>>) {
        let gen_next_move = samurai_move_positions;

        let n = self.n as usize;
        let mut dist = vec![vec![std::i32::MAX; n]; n];
        let mut prev = vec![vec![Action(-1); n]; n];
        let mut ques = vec![Vec::new(); 3];
        let mut que_idx = vec![0; 3];

        let mut qi: ModInt3 = 0.into();
        ques[0].push((s.clone(), 0));
        dist[s.y as usize][s.x as usize] = 0;
        let mut rest_que_size = 1;
        while rest_que_size > 0 {
            let qi_us = qi.us();
            while que_idx[qi_us] < ques[qi_us].len() {
                let p = ques[qi_us][que_idx[qi_us]].0;
                let d = ques[qi_us][que_idx[qi_us]].1;
                que_idx[qi_us] += 1;
                rest_que_size -= 1;
                gen_next_move(&p)
                    .into_iter()
                    .filter(|q| self.is_valid_pos(&q))
                    .filter(|q| !self.is_other_player_pos(&q))
                    .for_each(|q| {
                        let mut d = d + 1;
                        let mut prev_action = to_move_action(&p, &q);
                        if self.is_hole(&q) {
                            d += 1;
                            prev_action = prev_action.to_fill();
                        }
                        if self.is_gold(&q) {
                            prev_action = prev_action.to_dig();
                        }
                        if dist[p.y as usize][p.x as usize] != 0 {
                            prev_action = prev[p.y as usize][p.x as usize];
                        }
                        // eprintln!("check {} {}", dist[q.y as usize][q.x as usize], d);
                        if dist[q.y as usize][q.x as usize] > d
                            || dist[q.y as usize][q.x as usize] == d && rng.next() % 2 == 0
                        {
                            dist[q.y as usize][q.x as usize] = d;
                            prev[q.y as usize][q.x as usize] = prev_action;
                            ques[(qi + d.into()).us()].push((q, d));
                            rest_que_size += 1;
                        }
                    });
            }
            qi += 1.into();
        }
        (dist, prev)
    }

    pub fn calc_dist_by_dog_with_agent(
        &self,
        s: &P,
        rng: &mut Rng,
    ) -> (Vec<Vec<i32>>, Vec<Vec<Action>>) {
        let gen_next_move = dog_move_positions;

        let n = self.n as usize;
        let mut dist = vec![vec![std::i32::MAX; n]; n];
        let mut prev = vec![vec![Action(-1); n]; n];
        let mut ques = vec![Vec::new(); 3];
        let mut que_idx = vec![0; 3];

        let mut qi: ModInt3 = 0.into();
        ques[0].push((s.clone(), 0));
        dist[s.y as usize][s.x as usize] = 0;
        let mut rest_que_size = 1;
        while rest_que_size > 0 {
            let qi_us = qi.us();
            while que_idx[qi_us] < ques[qi_us].len() {
                let p = ques[qi_us][que_idx[qi_us]].0;
                let d = ques[qi_us][que_idx[qi_us]].1;
                que_idx[qi_us] += 1;
                rest_que_size -= 1;

                gen_next_move(&p)
                    .into_iter()
                    .filter(|q| self.is_valid_pos(&q))
                    .filter(|q| !self.agent.contains(&q))
                    .for_each(|q| {
                        let d = d + 1;
                        let mut prev_action = to_move_action(&p, &q);
                        if self.is_hole(&q) {
                            return;
                        }
                        if dist[p.y as usize][p.x as usize] != 0 {
                            prev_action = prev[p.y as usize][p.x as usize];
                        }
                        // eprintln!("check {} {}", dist[q.y as usize][q.x as usize], d);
                        if dist[q.y as usize][q.x as usize] > d
                            || dist[q.y as usize][q.x as usize] == d && rng.next() % 2 == 0
                        {
                            dist[q.y as usize][q.x as usize] = d;
                            prev[q.y as usize][q.x as usize] = prev_action;
                            ques[(qi + d.into()).us()].push((q, d));
                            rest_que_size += 1;
                        }
                    });
            }
            qi += 1.into();
        }
        (dist, prev)
    }

    pub fn calc_dist_by_dog(&self, s: &P, rng: &mut Rng) -> (Vec<Vec<i32>>, Vec<Vec<Action>>) {
        let gen_next_move = dog_move_positions;

        let n = self.n as usize;
        let mut dist = vec![vec![std::i32::MAX; n]; n];
        let mut prev = vec![vec![Action(-1); n]; n];
        let mut ques = vec![Vec::new(); 3];
        let mut que_idx = vec![0; 3];

        let mut qi: ModInt3 = 0.into();
        ques[0].push((s.clone(), 0));
        dist[s.y as usize][s.x as usize] = 0;
        let mut rest_que_size = 1;
        while rest_que_size > 0 {
            let qi_us = qi.us();
            while que_idx[qi_us] < ques[qi_us].len() {
                let p = ques[qi_us][que_idx[qi_us]].0;
                let d = ques[qi_us][que_idx[qi_us]].1;
                que_idx[qi_us] += 1;
                rest_que_size -= 1;

                gen_next_move(&p)
                    .into_iter()
                    .filter(|q| self.is_valid_pos(&q))
                    .for_each(|q| {
                        let d = d + 1;
                        let mut prev_action = to_move_action(&p, &q);
                        if self.is_hole(&q) {
                            return;
                        }
                        if dist[p.y as usize][p.x as usize] != 0 {
                            prev_action = prev[p.y as usize][p.x as usize];
                        }
                        // eprintln!("check {} {}", dist[q.y as usize][q.x as usize], d);
                        if dist[q.y as usize][q.x as usize] > d
                            || dist[q.y as usize][q.x as usize] == d && rng.next() % 2 == 0
                        {
                            dist[q.y as usize][q.x as usize] = d;
                            prev[q.y as usize][q.x as usize] = prev_action;
                            ques[(qi + d.into()).us()].push((q, d));
                            rest_que_size += 1;
                        }
                    });
            }
            qi += 1.into();
        }
        (dist, prev)
    }

    // fn calc_dist_between_all_cells(
    //     &self,
    //     gen_next_move: fn(&P) -> Vec<P>,
    // ) -> Vec<Vec<Vec<Vec<i32>>>> {
    //     let n = self.n as usize;
    //     let mut dist = vec![vec![vec![vec![std::i32::MAX; n]; n]; n]; n];
    //     self.all_cells().into_iter().for_each(|s| {
    //         let di = &mut dist[s.y as usize][s.x as usize];
    //         let mut que = Vec::new();
    //         que.push((s, 0));
    //         di[s.y as usize][s.x as usize] = 0;
    //         for i in 0..que.len() {
    //             let p = que[i].0;
    //             let d = que[i].1 + 1;
    //             gen_next_move(&p).into_iter().for_each(|q| {
    //                 if di[q.y as usize][q.x as usize] > d {
    //                     di[q.y as usize][q.x as usize] = d;
    //                     que.push((q, d));
    //                 }
    //             });
    //         }
    //     });
    //     dist
    // }

    // fn calc_dist_between_all_cells_by_samurai(&self) -> Vec<Vec<Vec<Vec<i32>>>> {
    //     self.calc_dist_between_all_cells(samurai_move_positions)
    // }

    // fn calc_dist_between_all_cells_by_dog(&self) -> Vec<Vec<Vec<Vec<i32>>>> {
    //     self.calc_dist_between_all_cells(dog_move_positions)
    // }

    fn is_valid_pos(&self, p: &P) -> bool {
        p.x >= 0 && p.x < self.n && p.y >= 0 && p.y < self.n
    }

    pub fn is_dog(&self) -> bool {
        self.player_index / 2 == 1
    }

    pub fn is_samurai(&self) -> bool {
        !self.is_dog()
    }

    pub fn all_cells(&self) -> Vec<P> {
        (0..self.n)
            .flat_map(|x| (0..self.n).map(move |y| P { x, y }))
            .collect()
    }

    // pub fn movable_area(&self) -> i32 {
    //     self.all_cells()
    //         .into_iter()
    //         .filter(|p| p.dist2(&self.agent[1]) > p.dist2(&self.agent[2]))
    //         .count() as i32
    // }

    fn calc_gold(&self, p: &P) -> i32 {
        self.public_gold
            .iter()
            .filter(|g| &g.p == p)
            .map(|g| g.quantity)
            .sum::<i32>()
            + self
                .private_gold
                .iter()
                .filter(|g| &g.p == p)
                .map(|g| g.quantity)
                .sum::<i32>()
    }

    // TODO ここ整理する
    fn act(&self, acts: [Action; 4]) -> Option<State> {
        // for i in 0..acts.len() {
        //     assert!(!(i / 2 == 1 && acts[i].is_dig_or_fill()));
        // }

        let mut prohibit_pos: Vec<_> = self.agent.to_vec();
        let mut next_pos = self.agent.clone();
        let mut actual_act = acts.clone();

        for i in 0..acts.len() {
            if !acts[i].is_dig_or_fill() {
                let p = acts[i].move_pos(&self.agent[i]);
                next_pos[i] = p;
                prohibit_pos.push(p);
            }
            assert!(i / 2 == 0 || !acts[i].is_dig_or_fill());
        }

        for i in 0..acts.len() {
            if !self.is_valid_pos(&next_pos[i]) {
                return None;
            }

            if acts[i].is_dig_or_fill() {
                continue;
            }

            if count(&prohibit_pos, &next_pos[i]) >= 2 || count(&self.hole, &next_pos[i]) >= 1 {
                next_pos[i] = self.agent[i].clone();
                actual_act[i] = Action::from(-1);
            }
        }

        prohibit_pos.clear();
        prohibit_pos.append(&mut self.agent.to_vec());
        prohibit_pos.append(&mut next_pos.to_vec());

        let mut dig_pos = Vec::new();

        for i in 0..acts.len() {
            if !acts[i].is_dig_or_fill() {
                continue;
            }

            let p = acts[i].target_pos(&self.agent[i]);

            if !self.is_valid_pos(&p) {
                return None;
            }

            if count(&prohibit_pos, &p) >= 1 || (acts[i].is_dig() != (count(&self.hole, &p) == 0)) {
                // let q = P { x: 5, y: 4 };
                // if p == q && acts[i].is_dig() {
                //     eprintln!(
                //         "cancel {:?} {:?} {:?} {:?}",
                //         prohibit_pos,
                //         p,
                //         acts[i],
                //         count(&self.hole, &p)
                //     );
                // }
                actual_act[i] = Action::from(-1);
            }

            if actual_act[i].is_dig() {
                dig_pos.push(p);
            }
        }

        // if !dig_pos.is_empty() {
        //     eprintln!("dig_pos {:?}", dig_pos);
        //     eprintln!("acts {:?}", acts);
        //     eprintln!("actual_act {:?}", actual_act);
        // }

        let mut state = self.clone();
        state.score.iter_mut().for_each(|s| *s = 0);
        for i in 0..actual_act.len() {
            state.agent[i] = next_pos[i].clone();
            if actual_act[i].is_dig() {
                let p = acts[i].target_pos(&self.agent[i]);
                state.score[i % 2] += self.calc_gold(&p) / count(&dig_pos, &p) as i32;
            }
        }

        state.histories.push(acts);

        Some(state)
    }

    pub fn next_states(&self) -> Vec<State> {
        let mut states = Vec::new();
        for s1 in Action::samurai_actions() {
            for d1 in Action::dog_actions() {
                for s2 in Action::samurai_actions() {
                    for d2 in Action::dog_actions() {
                        if let Some(s) = self.act([s1.clone(), s2.clone(), d1.clone(), d2.clone()])
                        {
                            states.push(s);
                        }
                    }
                }
            }
        }
        states
    }
}

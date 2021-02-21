mod core;
mod delegate_plan;
mod greedy_dog_plan;
mod greedy_plan;
mod mod_int;
mod obstruction_plan;
mod random_plan;
mod rng;
mod scanner;

use self::core::{Action, Gold, Plan, State, P};
use self::delegate_plan::DeleatePlan;
// use self::greedy_dog_plan::GreedyDogPlan;
use self::greedy_plan::GreedyPlan;
use self::obstruction_plan::ObstructionPlan;
// use self::random_plan::RandomPlan;
use self::scanner::Scanner;

// struct EnemyPlanEstimator {}

fn read_p(sc: &mut Scanner) -> P {
    let x = sc.next();
    let y = sc.next();
    P { x, y }
}

fn read_gold(sc: &mut Scanner) -> Gold {
    let x = sc.next();
    let y = sc.next();
    let quantity = sc.next();
    Gold {
        p: P { x, y },
        quantity,
    }
}

fn read_action(sc: &mut Scanner) -> Action {
    Action(sc.next())
}

fn read_vec<T>(sc: &mut Scanner, f: fn(&mut Scanner) -> T) -> Vec<T> {
    let n = sc.next();
    (0..n).map(|_| f(sc)).collect()
}

fn read_4<T: Default + Copy>(sc: &mut Scanner, f: fn(&mut Scanner) -> T) -> [T; 4] {
    let mut res = [Default::default(); 4];
    res.iter_mut().for_each(|v| *v = f(sc));
    res
}

fn read_2<T: Default + Copy>(sc: &mut Scanner, f: fn(&mut Scanner) -> T) -> [T; 2] {
    let mut res = [Default::default(); 2];
    res.iter_mut().for_each(|v| *v = f(sc));
    res
}

fn read_turn_info() -> State {
    // let mut state: State = Default::default();
    let stdin = std::io::stdin();
    let mut sc = Scanner::new(stdin.lock());

    let player_index: i32 = sc.next();
    let n: i32 = sc.next();
    let step: usize = sc.next();
    let max_step: usize = sc.next();
    let hole: Vec<P> = read_vec(&mut sc, read_p);
    let public_gold: Vec<Gold> = read_vec(&mut sc, read_gold);
    let private_gold: Vec<Gold> = read_vec(&mut sc, read_gold);
    let agent: [P; 4] = read_4(&mut sc, read_p);
    let try_plan: [Action; 4] = read_4(&mut sc, read_action);
    let actual_plan: [Action; 4] = read_4(&mut sc, read_action);
    let score: [i32; 2] = read_2(&mut sc, |sc| sc.next());
    let rest_total_gold: i32 = sc.next();
    let rest_think_time: i32 = sc.next();
    let histories: Vec<[Action; 4]> = vec![];

    State {
        player_index,
        n,
        step,
        max_step,
        hole,
        public_gold,
        private_gold,
        agent,
        try_plan,
        actual_plan,
        score,
        rest_total_gold,
        rest_think_time,
        histories,
    }
}

fn main() {
    let mut plan = DeleatePlan {
        samurai_plan: Box::new(GreedyPlan::new()),
        // dog_plan: Box::new(GreedyDogPlan::new()),
        dog_plan: Box::new(ObstructionPlan::new()),
    };

    loop {
        let state = read_turn_info();
        let action = plan.decide(&state);
        // eprintln!("{:?}", state.calc_dist_by_samurai(&P { x: 3, y: 3 }));
        println!("{}", action);
    }
}

#[test]
fn test() {
    use self::mod_int::{ext_gcd, ModInt3};
    // let mut modular: ModInt3 = ModInt3::from(100);
    let mut modular: ModInt3 = 100.into();
    for _ in 0..10 {
        modular += 1.into();
        let g = ext_gcd(10, 3);
        eprintln!("{} {:?} {:?}", modular, modular, g);
    }
}

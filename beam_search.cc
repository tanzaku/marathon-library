// union find
#include <queue>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <memory>

using namespace std;


struct BeamState {
	shared_ptr<BeamState> prev;

	// 評価値が高い状態が先に来るように定義する
	bool operator<(const BeamState& rhs) const {

	}
};

using S = shared_ptr<BeamState>;

void chokudai() {
	priority_queue<S, vector<S>, greater<S>> que[MAX_TURN+1];

	S best;
	while (true) {
		for (int i = 0; i < MAX_TURN; i++) {
			auto s = que[i].top(); qur[i].pop();
			for (S t : s.extract()) {
				if (best->score < t->score) {
					best = t;
				}
				que[i+1].push(t);
			}
		}
	}
}

void beamSearch() {
	vector<S> cur, next;

	// S best;
	for (int i = 0; i < MAX_TURN; i++) {
		for (auto s : cur) {
			for (S t : s.extract()) {
				// if (best > t) { best = t; }
				next.push_back(t);
			}
		}
		if (next.size() > BEAM_WIDTH) {
			nth_element(next.begin(), next.begin() + BEAM_WIDTH, next.end());
			next.resize(BEAM_WIDTH);
		}
		cur.clear();
		swap(cur, next);
	}
	return *min_element(cur.begin(), cur.end());
}

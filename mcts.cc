// union find
#include <queue>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <cmath>

using namespace std;

struct Node {
	double sum;
	int times;
	vector<Node> children;

	Node* choice() {
		double best = -1e300;
		Node* choiced = nullptr;
		for (auto& child : children) {
			double value = child.ucb1(times);
			if (best < value) {
				best = value;
				choiced = &child;
			}
		}
		return choiced;
	}

	double ucb1(int parentVisitTimes) {
		if (times == 0) {
			return 1e300;
		}
		const double mean = sum / times;
		const double c = 1;
		return  mean + c * std::sqrt(2 * std::log(parentVisitTimes) / times);
	}

	void update(double v) {
		times += 1;
		sum += v;
	}

	double evaluate() {

	}

	void expand() {

	}
};

void rollout(Node& root) {
	vector<Node*> nodes;
	for (Node* node = &root; node; node = node->choice()) {
		nodes.push_back(node);
	}
	nodes.back()->expand();
	double v = nodes.back()->evaluate();
	for (auto& node : nodes) {
		node->update(v);
	}
}

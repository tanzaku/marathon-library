// MCF
#include <queue>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>

using namespace std;

template<typename W, typename C>
struct Edge {
public:
	int to;
	C cap;
	W cost;
	int rev;

	Edge() {}
	Edge(int to, C cap, W cost, int rev)
		: to(to)
		, cap(cap)
		, cost(cost)
		, rev(rev) {
	}
};

template<typename W, typename C, int V>
class MinCostFlow {
public:
	array<vector<Edge<W, C>>, V> g;
	array<W, V> h, dist;
	array<int, V> prevv, preve;

	const W INF = 1 << 29;

	void add_edge(int from, int to, C cap, W cost) {
		g[from].emplace_back(to, cap, cost, g[to].size());
		g[to].emplace_back(from, 0, -cost, g[from].size() - 1);
	}

	W min_cost_flow(int s, int t, C f) {
		int res = 0;
		fill(h.begin(), h.end(), 0);
		while (f > 0) {
			fill(dist.begin(), dist.end(), INF);
			dist[s] = 0;
			priority_queue<tuple<W, int>, vector<tuple<W, int>>, greater<tuple<W, int>>> q;
			q.emplace(0, s);
			while (!q.empty()) {
				W p;
				int v;
				tie(p, v) = q.top(); q.pop();
				if (dist[v] < p) continue;

				for (int i = 0; i < static_cast<int>(g[v].size()); i++) {
					auto& e = g[v][i];
					int dis = dist[v] + e.cost + h[v] - h[e.to];
					if (e.cap > 0 && dist[e.to] > dis) {
						dist[e.to] = dis;
						prevv[e.to] = v;
						preve[e.to] = i;
						q.emplace(dist[e.to], e.to);
					}
				}
			}
			if (dist[t] == INF) return -1;
			for (int i = 0; i < V; i++) h[i] += dist[i];

			C d = f;
			for (int i = t; i != s; i = prevv[i])
				d = min<C>(d, g[prevv[i]][preve[i]].cap);

			f -= d;
			res += d * h[t];
			for (int i = t; i != s; i = prevv[i]) {
				auto& e = g[prevv[i]][preve[i]];
				e.cap -= d;
				g[i][e.rev].cap += d;
			}
		}
		return res;
	}
};
// MCF

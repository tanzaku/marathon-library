
#include <cmath>
#include <cstdint>
#include <tuple>
#include <vector>
#include <queue>

using namespace std;

struct Edge {
  int w, t;
};

struct Graph {
  vector<vector<Edge>> g;

  void dijkstra(int s)
  {
    priority_queue<tuple<int, int>, vector<tuple<int, int>>, greater<tuple<int, int>>> que;
    que.emplace(s, 0);
    vector<int> dist(g.size(), 1 << 29);
    vector<int> prev(g.size(), -1);
    dist[s] = 0;
    while (!que.empty()) {
      int v, d;
      tie(v, d) = que.top();
      que.pop();
      if (dist[v] != d) continue;
      for (auto &e : g[v]) {
        int w = d + e.w;
        if (dist[e.t] <= w) continue;
        dist[e.t] = w;
        prev[e.t] = v;
        que.emplace(e.t, w);
      }
    }
  }

  void bfs(int s)
  {
    vector<int> que;
    que.emplace_back(s);
    vector<int> dist(g.size(), 1 << 29);
    vector<int> prev(g.size(), -1);
    dist[s] = 0;
    for (int i = 0; i < (int)que.size(); i++) {
      int v = que[i];
      for (auto &e : g[v]) {
        assert(e.w == 1);
        int w = dist[v] + 1;
        if (dist[e.t] <= w) continue;
        dist[e.t] = w;
        prev[e.t] = v;
        que.emplace_back(e.t);
      }
    }
  }

  void bfs01(int s)
  {
    deque<tuple<int, int>> deq;
    deq.emplace_back(s, 0);
    vector<int> dist(g.size(), 1 << 29);
    vector<int> prev(g.size(), -1);
    while (!deq.empty()) {
      int v, d;
      tie(v, d) = deq.front();
      deq.pop_front();
      if (dist[v] != d) continue;
      for (auto &e : g[v]) {
        assert(e.w == 0 || e.w == 1);
        int w = d + e.w;
        if (dist[e.t] <= w) continue;
        dist[e.t] = w;
        prev[e.t] = v;
        if (e.w == 0)
          deq.emplace_front(e.t, w);
        else
          deq.emplace_back(e.t, w);
      }
    }
  }
};

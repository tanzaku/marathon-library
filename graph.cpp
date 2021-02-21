
#include <cmath>
#include <cstdint>
#include <queue>
#include <tuple>
#include <vector>

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

void spfa()
{
  for (int s = 0; s < int(g.size()); s++) {
    auto &d = dist[s];
    fill(begin(d), end(d), inf);
    d[s] = 0;
    que.push_back(s);
    for (int i = 0; i < int(que.size()); i++) {
      const int v = que[i];
      inQue[v] = 0;
      for (auto pr : g[v]) {
        int t = get<0>(pr);
        int c = get<1>(pr);
        if (d[t] > d[v] + c) {
          d[t] = d[v] + c;
          if (!inQue[t]) {
            inQue[t] = 1;
            que.push_back(t);
          }
        }
      }
    }
    que.clear();
  }
}

void warshallFloyd(vector<vector<int>> &d)
{
  for (int k = 0; k < int(d.size()); k++) {
    for (int i = 0; i < int(d.size()); i++) {
      for (int j = 0; j < int(d.size()); j++) {
        d[i][j] = std::min(d[i][j], d[i][k] + d[k][j]);
      }
    }
  }
}

template<typename W>
struct Edge {
public:
  int to;
  W cap;
  int rev;

  Edge(int to, W cap, int rev)
    : to(to)
    , cap(cap)
    , rev(rev) {
  }
};

template<typename W, int V>
class Dinic {
public:
  vector<Edge<W> > g[V];
  int level[V];
  int iter[V];
  int q[V];

  void bfs(int s) {
    std::fill(level, level + V, -1);

    int qs = 0, qt = 0;
    level[s] = 0;
    q[qt++] = s;

    while(qs != qt) {
      int v = q[qs++];
      for(int i = 0; i < g[v].size(); i++) {
        Edge<W> &e = g[v][i];
        if(e.cap > 0 && level[e.to] < 0) {
          level[e.to] = level[v] + 1;
          q[qt++] = e.to;
        }
      }
    }
  }

  int dfs(int v, int t, W f) {
    if(v == t) return f;
    for(int &i = iter[v]; i < g[v].size(); i++) {
      Edge<W> &e = g[v][i];
      if(e.cap > 0 && level[v] < level[e.to]) {
        int d = dfs(e.to, t, std::min(f, e.cap));
        if(d > 0) {
          e.cap -= d;
          g[e.to][e.rev].cap += d;
          return d;
        }
      }
    }
    return 0;
  }

  void addEdge(int s, int t, int cap) {
    g[s].push_back(Edge<W>(t, cap, g[t].size()));
    g[t].push_back(Edge<W>(s, 0, g[s].size() - 1));
  }

  int maxFlow(int s, int t, W INF) {
    int flow = 0;
    while(true) {
      bfs(s);
      if(level[t] < 0) return flow;
      std::fill(iter, iter + V, 0);
      for(int f = 0; (f = dfs(s, t, INF)) > 0; ) {
        flow += f;
      }
      // cout << "flow: " << flow << endl;
    }
  }
};

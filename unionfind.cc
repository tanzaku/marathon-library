// union find
#include <queue>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>

using namespace std;


struct union_find {
	vector<int> data;
	
	union_find(int size) : data(size, -1) {
	}
	
	int root(int x) { return data[x] < 0 ? x : (data[x] = root(data[x])); }
	
	void unite(int x, int y) {
		if((x = root(x)) != (y = root(y))) {
			if(data[y] < data[x]) { swap(x, y); }
			data[x] += data[y];
			data[y] = x;
		}
	}
	
	bool same(int x, int y) { return root(x) == root(y); }
	int size(int x) { return -data[root(x)]; }
};

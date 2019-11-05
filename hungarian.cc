// union find
#include <queue>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include <numeric>

using namespace std;


pair<int, vector<pair<int,int>>> hungarian(vector<vector<int>> weights) {
	const int n = (int)weights.size();
	assert (weights[0].size() == n);
	vector<int> rowIdx(n);
	vector<int> columnIdx(n);
	for (int i = 0; i < n; i++) {
		columnIdx[i] = rowIdx[i] = i;
	}
	vector<pair<int,int>> assignment;
	while (true) {
		for (int y = 0; y < (int)rowIdx.size(); y++) {
			int lowest = std::numeric_limits<int>::max();
			for (int x = 0; x < (int)columnIdx.size(); x++) lowest = std::min(lowest, weights[y][x]);
			for (int x = 0; x < (int)columnIdx.size(); x++) weights[y][x] -= lowest;
		}
		for (int x = 0; x < (int)columnIdx.size(); x++) {
			int lowest = std::numeric_limits<int>::max();
			for (int y = 0; y < (int)rowIdx.size(); y++) lowest = std::min(lowest, weights[y][x]);
			for (int y = 0; y < (int)rowIdx.size(); y++) weights[y][x] -= lowest;
		}
		for (int y = 0; y < (int)rowIdx.size(); y++) {
			for (int x = 0; x < (int)columnIdx.size(); x++) if (columnIdx[x] >= 0) {
				if (weights[y][x] == 0) {
					assignment.emplace_back(rowIdx[y], columnIdx[x]);
					rowIdx[y] = columnIdx[x] = -1;
					break;
				}
			}
		}
		int r = 0;
		for (int y = 0; y < (int)rowIdx.size(); y++) {
			if (rowIdx[y] >= 0 && y != r++) {
				swap(rowIdx[y], rowIdx[r-1]);
				swap(weights[y], weights[r-1]);
			}
		}
		rowIdx.resize(r);
		weights.resize(r);
		int c = 0;
		for (int x = 0; x < (int)columnIdx.size(); x++) {
			if (columnIdx[x] >= 0 && x!= c++) {
			for (int y = 0; y < (int)rowIdx.size(); y++) {

			}
			}
		}
	}
}

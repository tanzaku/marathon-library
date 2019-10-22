#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

vector<string> split(string s, char delimiter)
{
  istringstream stream(s);

  string field;
  vector<string> result;
  while (getline(stream, field, delimiter)) {
    result.push_back(field);
  }
  return result;
}

vector<int> split_int(string s, char delimiter)
{
  istringstream stream(s);

  string field;
  vector<int> result;
  while (getline(stream, field, delimiter)) {
    result.push_back(atoi(field.data()));
  }
  return result;
}

template <typename T>
string join(T &s, char delimiter)
{
  string result;
  for (int i = 0; i < (int)s.size(); i++) {
    if (i != 0) result += delimiter;
    result += to_string(s[i]);
  }
  return result;
}

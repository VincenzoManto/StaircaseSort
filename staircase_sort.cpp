#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <limits>
#include <random>
#include <iomanip>
#include <numeric>

using namespace std;
using namespace std::chrono;

// --- STAIRCASE SORT ---
template <typename T>
void staircaseSort(vector<T>& arr) {
    int n = arr.size();
    if (n <= 1) return;

    T min_v = arr[0], max_v = arr[0];
    for (const auto& x : arr) {
        if (x < min_v) min_v = x;
        if (x > max_v) max_v = x;
    }

    if (min_v == max_v) return;

    int size = static_cast<int>(n * 1.6);
    const T EMPTY = numeric_limits<T>::max();
    vector<T> staircase(size, EMPTY);

    double range = static_cast<double>(max_v - min_v);
    double multiplier = (size - 1) / range;

    for (T x : arr) {
        int idx = static_cast<int>((x - min_v) * multiplier);
        while (true) {
            if (staircase[idx] == EMPTY) {
                staircase[idx] = x;
                break;
            } else {
                if (x < staircase[idx]) swap(x, staircase[idx]);
                idx++;
                if (idx >= staircase.size()) {
                    staircase.push_back(x);
                    break;
                }
            }
        }
    }

    int current = 0;
    for (const auto& v : staircase) {
        if (v != EMPTY) arr[current++] = v;
    }
}

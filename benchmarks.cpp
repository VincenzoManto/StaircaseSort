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

    // We can use a scale factor 1.6 for collisions
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

// --- BENCHMARK ---
// Editor note: suggest me other benchmarks pls
struct Stats {
    double avg, min_t, max_t;
};

Stats runBenchmark(const string& name, vector<double>& original_data, int iterations, void (*sortFunc)(vector<double>&)) {
    vector<double> times;
    for (int i = 0; i < iterations; ++i) {
        vector<double> data = original_data; // Copia dati freschi
        auto start = high_resolution_clock::now();
        sortFunc(data);
        auto end = high_resolution_clock::now();
        
        if (i == 0 && !is_sorted(data.begin(), data.end())) {
            cout << "ERROR: " << name << " it's not even sorting!" << endl;
        }

        duration<double, milli> diff = end - start;
        times.push_back(diff.count());
    }

    double sum = accumulate(times.begin(), times.end(), 0.0);
    double min_v = *min_element(times.begin(), times.end());
    double max_v = *max_element(times.begin(), times.end());
    return {sum / iterations, min_v, max_v};
}

// Wrapper for std::sort
void stdSortWrapper(vector<double>& v) { sort(v.begin(), v.end()); }
void stdStableSortWrapper(vector<double>& v) { stable_sort(v.begin(), v.end()); }

void printRow(string name, Stats s) {
    cout << left << setw(20) << name 
         << fixed << setprecision(4) << setw(15) << s.avg 
         << setw(15) << s.min_t 
         << setw(15) << s.max_t << " ms" << endl;
}

int main() {
    const int N = 50000;      
    const int ITERATIONS = 50; 
    
    mt19937 rng(42);
    uniform_real_distribution<double> dist(0.0, 100000.0);

    cout << "===========================================================" << endl;
    cout << "BENCHMARK: STAIRCASE SORT vs STANDARD ALGORITHMS" << endl;
    cout << "Data size: " << N << " | Iterations: " << ITERATIONS << " per sort" << endl;
    cout << "===========================================================" << endl;
    cout << left << setw(20) << "Algorithm" << setw(15) << "AVG" << setw(15) << "MIN" << setw(15) << "MAX" << endl;
    cout << "-----------------------------------------------------------" << endl;

    // --- TEST 1: random data ---
    vector<double> random_data(N);
    for(int i=0; i<N; ++i) random_data[i] = dist(rng);

    cout << "\n[Randoms]" << endl;
    printRow("Staircase Sort", runBenchmark("Staircase", random_data, ITERATIONS, staircaseSort));
    printRow("std::sort", runBenchmark("std::sort", random_data, ITERATIONS, stdSortWrapper));
    printRow("std::stable_sort", runBenchmark("std::stable", random_data, ITERATIONS, stdStableSortWrapper));

    // --- TEST 2: ordered data ---
    vector<double> sorted_data = random_data;
    sort(sorted_data.begin(), sorted_data.end());

    cout << "\n[Ordered]" << endl;
    printRow("Staircase Sort", runBenchmark("Staircase", sorted_data, ITERATIONS, staircaseSort));
    printRow("std::sort", runBenchmark("std::sort", sorted_data, ITERATIONS, stdSortWrapper));

    // --- TEST 3: reversed data ---
    vector<double> reverse_data = sorted_data;
    reverse(reverse_data.begin(), reverse_data.end());

    cout << "\n[Reverse]" << endl;
    printRow("Staircase Sort", runBenchmark("Staircase", reverse_data, ITERATIONS, staircaseSort));
    printRow("std::sort", runBenchmark("std::sort", reverse_data, ITERATIONS, stdSortWrapper));

    cout << "===========================================================" << endl;
    return 0;
}

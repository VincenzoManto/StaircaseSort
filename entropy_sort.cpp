#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <limits>
#include <random>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <type_traits>

using namespace std;
using namespace std::chrono;

// --- Fallback ---
template<typename T>
void manual_partition_sort(std::vector<T>& arr, int start, int end) {
    if (end - start <= 1) return;
    T pivot = arr[start + (end - start) / 2];
    int i = start, j = end - 1;
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++; j--;
        }
    }
    if (start < j) manual_partition_sort(arr, start, j + 1);
    if (i < end) manual_partition_sort(arr, i, end);
}

template<typename T>
void resolve_entropy(std::vector<T>& input, int start, int end, int depth) {
    int n = end - start;
    if (n <= 1) return;

    T min_v = input[start], max_v = input[start];
    for (int i = start + 1; i < end; ++i) {
        if (input[i] < min_v) min_v = input[i];
        if (input[i] > max_v) max_v = input[i];
    }

    if (min_v == max_v) return;

    if constexpr (std::is_integral<T>::value) {
        long long range_span = (long long)max_v - (long long)min_v;
        if (range_span < n && range_span < 100000) {
            std::vector<int> counts(range_span + 1, 0);
            for (int i = start; i < end; ++i) counts[input[i] - min_v]++;
            int curr = start;
            for (int i = 0; i <= range_span; ++i) {
                while (counts[i]--) input[curr++] = (T)(i + min_v);
            }
            return;
        }
    }

    if (depth > 15) {
        manual_partition_sort(input, start, end);
        return;
    }

    int num_buckets = std::max(2, (int)std::sqrt(n) + 2);
    std::vector<std::vector<T>> buckets(num_buckets);

    double range_span = (double)max_v - (double)min_v;

    for (int i = start; i < end; ++i) {
        double normalized = (double)(input[i] - min_v) / range_span;
        int b_idx = (int)(normalized * (num_buckets - 1));
        if (b_idx < 0) b_idx = 0;
        if (b_idx >= num_buckets) b_idx = num_buckets - 1;
        buckets[b_idx].push_back(input[i]);
    }

    int curr = start;
    for (int b = 0; b < num_buckets; ++b) {
        if (buckets[b].empty()) continue;
        int b_start = curr;
        for (T val : buckets[b]) input[curr++] = val;
        
        if (buckets[b].size() > 1) {
            resolve_entropy(input, b_start, curr, depth + 1);
        }
    }
}

template<typename T>
void entropySort(vector<T>& arr) {
    if (arr.empty()) return;
    resolve_entropy(arr, 0, arr.size(), 0);
}

// --- BENCHMARK SUITE ---
struct Stats {
    double avg, min_t, max_t;
};

template<typename T>
Stats runBenchmark(const string& name, vector<T>& original_data, int iterations, void (*sortFunc)(vector<T>&)) {
    vector<double> times;
    for (int i = 0; i < iterations; ++i) {
        vector<T> data = original_data; 
        auto start = high_resolution_clock::now();
        sortFunc(data);
        auto end = high_resolution_clock::now();

        if (i == 0 && !is_sorted(data.begin(), data.end())) {
            cout << "ERROR: " << name << "Not working" << endl;
        }

        duration<double, milli> diff = end - start;
        times.push_back(diff.count());
    }
    double sum = accumulate(times.begin(), times.end(), 0.0);
    return {sum / iterations, *min_element(times.begin(), times.end()), *max_element(times.begin(), times.end())};
}

void stdSortWrapper(vector<double>& v) { sort(v.begin(), v.end()); }
void stdStableSortWrapper(vector<double>& v) { stable_sort(v.begin(), v.end()); }

void printRow(string name, Stats s) {
    cout << left << setw(20) << name << fixed << setprecision(4) 
         << setw(15) << s.avg << setw(15) << s.min_t << setw(15) << s.max_t << " ms" << endl;
}

int main() {
    const int N = 50000;      
    const int ITERATIONS = 10; 

    mt19937 rng(42);
    uniform_real_distribution<double> dist(0.0, 100000.0);

    cout << "===========================================================" << endl;
    cout << "BENCHMARK: Entropy SORT vs STANDARD ALGORITHMS" << endl;
    cout << "===========================================================" << endl;
    cout << left << setw(20) << "Algorithm" << setw(15) << "AVG" << setw(15) << "MIN" << setw(15) << "MAX" << endl;

    vector<double> random_data(N);
    for(int i=0; i<N; ++i) random_data[i] = dist(rng);
    cout << "\n[DISTRIBUZIONE: CASUALE]" << endl;
    printRow("Entropy Sort", runBenchmark("Entropy", random_data, ITERATIONS, entropySort));
    printRow("std::sort", runBenchmark("std::sort", random_data, ITERATIONS, stdSortWrapper));

    vector<double> sorted_data = random_data;
    sort(sorted_data.begin(), sorted_data.end());
    cout << "\n[DISTRIBUZIONE: GIA' ORDINATI]" << endl;
    printRow("Entropy Sort", runBenchmark("Entropy", sorted_data, ITERATIONS, entropySort));
    printRow("std::sort", runBenchmark("std::sort", sorted_data, ITERATIONS, stdSortWrapper));

    uniform_real_distribution<double> narrow_dist(0.0, 10.0);
    vector<double> narrow_data(N);
    for(int i=0; i<N; ++i) narrow_data[i] = narrow_dist(rng);
    cout << "\n[Narrow Range - Many Collisions]" << endl;
    printRow("Entropy Sort", runBenchmark("Entropy", narrow_data, ITERATIONS, entropySort));
    printRow("std::sort", runBenchmark("std::sort", narrow_data, ITERATIONS, stdSortWrapper));
    normal_distribution<double> gauss_dist(50000.0, 5000.0);
    vector<double> gaussian_data(N);
    for(int i=0; i<N; ++i) gaussian_data[i] = gauss_dist(rng);
    cout << "\n[Gaussian Distribution]" << endl;
    printRow("Entropy Sort", runBenchmark("Entropy", gaussian_data, ITERATIONS, entropySort));
    printRow("std::sort", runBenchmark("std::sort", gaussian_data, ITERATIONS, stdSortWrapper));
    printRow("std::stable_sort", runBenchmark("std::stable", gaussian_data, ITERATIONS, stdStableSortWrapper));
   
    
    // TEST 5: Quasi tutti uguali
    vector<double> mostly_same(N, 42.0);
    for(int i=0; i<N/100; ++i) mostly_same[rng() % N] = dist(rng);
    
    cout << "\n[Mostly Equal Values]" << endl;
    printRow("Entropy Sort", runBenchmark("Entropy", mostly_same, ITERATIONS, entropySort));
    printRow("std::sort", runBenchmark("std::sort", mostly_same, ITERATIONS, stdSortWrapper));
    
    return 0;
}

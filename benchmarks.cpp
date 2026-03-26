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
    
    double range = static_cast<double>(max_v - min_v);
    
    // macro-bucket
    int num_buckets = max(1, static_cast<int>(sqrt(n)));
    vector<vector<T>> buckets(num_buckets);
    
    double bucket_range = range / num_buckets;
    
    for (const T& x : arr) {
        int bucket_idx = min(num_buckets - 1, 
                            static_cast<int>((x - min_v) / bucket_range));
        buckets[bucket_idx].push_back(x);
    }
    
    // staircase for each bucket
    int current = 0;
    for (auto& bucket : buckets) {
        if (bucket.empty()) continue;
        
        int bucket_size = bucket.size();
        if (bucket_size == 1) {
            arr[current++] = bucket[0];
            continue;
        }
        
        T bucket_min = *min_element(bucket.begin(), bucket.end());
        T bucket_max = *max_element(bucket.begin(), bucket.end());
        
        if (bucket_min == bucket_max) {
            for (const T& val : bucket) arr[current++] = val;
            continue;
        }
        
        double bucket_sub_range = static_cast<double>(bucket_max - bucket_min);
        int stair_size = static_cast<int>(bucket_size * 1.8);
        
        const T EMPTY = numeric_limits<T>::max();
        vector<T> mini_stair(stair_size, EMPTY);
        
        double multiplier = (stair_size - 1) / bucket_sub_range;
        
        for (const T& x : bucket) {
            int idx = static_cast<int>((x - bucket_min) * multiplier);
            
           // probing
            while (idx < mini_stair.size()) {
                if (mini_stair[idx] == EMPTY) {
                    mini_stair[idx] = x;
                    break;
                } else if (x < mini_stair[idx]) {
                    swap(mini_stair[idx], const_cast<T&>(x));
                }
                idx++;
            }
            
            // overflow handling
            if (idx >= mini_stair.size()) {
                mini_stair.push_back(x);
            }
        }
        
        // reconstruct
        for (const T& v : mini_stair) {
            if (v != EMPTY) arr[current++] = v;
        }
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

    uniform_real_distribution<double> narrow_dist(0.0, 100.0); 
    vector<double> narrow_data(N);
    for(int i=0; i<N; ++i) narrow_data[i] = narrow_dist(rng);
    
    cout << "\n[Narrow range]" << endl;
    printRow("Staircase Sort", runBenchmark("Staircase", narrow_data, ITERATIONS, staircaseSort));
    printRow("std::sort", runBenchmark("std::sort", narrow_data, ITERATIONS, stdSortWrapper));
    
    // guassian
    normal_distribution<double> gauss_dist(50000.0, 5000.0);
    vector<double> gaussian_data(N);
    for(int i=0; i<N; ++i) gaussian_data[i] = gauss_dist(rng);
    cout << "\n[Gaussian]" << endl;
    printRow("Staircase Sort", runBenchmark("Staircase", gaussian_data, ITERATIONS, staircaseSort));
    printRow("std::sort", runBenchmark("std::sort", gaussian_data, ITERATIONS, stdSortWrapper));
   
    
    //  equals
    vector<double> mostly_same(N, 42.0);
    for(int i=0; i<N/100; ++i) mostly_same[rng() % N] = dist(rng);
    
    cout << "\n[Mostly equal values]" << endl;
    printRow("Staircase Sort", runBenchmark("Staircase", mostly_same, ITERATIONS, staircaseSort));
    printRow("std::sort", runBenchmark("std::sort", mostly_same, ITERATIONS, stdSortWrapper));
    

    cout << "===========================================================" << endl;
    return 0;
}

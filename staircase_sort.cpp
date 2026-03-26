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

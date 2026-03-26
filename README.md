# Staircase Sort 
### A Linear-Projection Mapping Algorithm that outperforms `std::sort` by 4x.

> **Note:** This algorithm was born as a "thought experiment", so a personal challenge to find a simpler, faster way to sort numbers by treating them as positions on a physical staircase rather than abstract values to compare. It resulted in a surprisingly efficient $O(n)$ average-case performance.

---

## Benchmarks
In benchmarks ([benchmark.cpp](benchmark)) involving **50,000 double-precision floating-point numbers**, Staircase Sort consistently crushes the industry-standard `std::sort` (Introsort) and `std::stable_sort` (Timsort/MergeSort derivative).

| Algorithm | Average Time (50k elements) | Performance Gain |
| :--- | :--- | :--- |
| **Staircase Sort** | **3.75 ms** | **Baseline** |
| `std::sort` | 14.87 ms | ~4.0x Slower |
| `std::stable_sort` | 17.07 ms | ~4.5x Slower |

*Tested on random distribution using C++ with `-O3` optimization.*

---

## "Domino" logic
Unlike QuickSort or MergeSort, which rely on $O(n \log n)$ comparisons, Staircase Sort uses **linear projection mapping**. 

1. **scan:** It finds the `min` and `max` of the dataset in a single pass.
2. **staircase:** It allocates a "staircase" (a contiguous memory buffer) typically $1.6 \times$ the size of the input.
3. **mapping:** Every number $x$ is mapped to a "step" (index) using the formula:  
   `index = (x - min) * multiplier`
4. **domino effect:** (for collision handling) 
   - If a step is empty, the number sits there.
   - If a step is occupied, the algorithm compares the new value with the existing one. The smaller value stays, and the larger one is "pushed" to the next step (like a falling domino). This keeps the staircase sorted locally during insertion.
5. **compaction:** The algorithm traverses the staircase once, skipping empty steps to return the sorted array.

---

## Obvious trade-offs
- **space complexity:** $O(n)$ as it uses extra memory (the staircase) to achieve its speed.
- **time complexity:** 
  - **Average/Best:** $O(n)$ for uniform distributions (mainly 1.6 * 2 * n iterations).
  - **worst:** $O(n^2)$ for highly pathological or clustered data (but i think it can be mitigated with a fallback).
- **stability:** Current implementation is **not stable!**.

---

## Usage
To run the benchmark yourself:

1. Clone the repo.
2. Compile with high optimization:
   ```bash
   g++ -O3 -march=native benchmark.cpp -o benchmark
   ```
3. Run the executable:
   ```bash
   ./benchmark
   ```

---

## 📜 License
MIT - Use it, break it, make it faster.

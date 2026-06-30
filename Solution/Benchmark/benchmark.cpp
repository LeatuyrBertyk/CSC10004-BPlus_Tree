#include "benchmark.h"

#include <chrono>
#include <iostream>

#include "../PageManipulation/Query/query.h"
#include "../global_variables.h"

using namespace std;

void resetIOCounters() {
  disk_read_count = 0;
  disk_write_count = 0;
}

void runBenchmarkMode(int mode, const int query_ids[], int range_size) {
  bool use_binary_search = true;
  bool use_linear_search = false;

  resetIOCounters();

  long long total_found = 0;
  auto start_time = chrono::high_resolution_clock::now();

  if (mode == 1) {
    for (int i = 0; i < query_count; i++) {
      total_found += pointQueryBTreeStyle(query_ids[i], use_linear_search);
    }
  } else if (mode == 2) {
    for (int i = 0; i < query_count; i++) {
      total_found += pointQueryBTreeStyle(query_ids[i], use_binary_search);
    }
  } else if (mode == 3) {
    for (int i = 0; i < query_count; i++) {
      total_found += pointQueryBPlusStyle(query_ids[i], use_linear_search);
    }
  } else if (mode == 4) {
    for (int i = 0; i < query_count; i++) {
      total_found += pointQueryBPlusStyle(query_ids[i], use_binary_search);
    }
  } else if (mode == 5) {
    for (int i = 0; i < query_count; i++) {
      total_found += rangeQueryBTreeStyle(
          query_ids[i], query_ids[i] + range_size, use_linear_search);
    }
  } else if (mode == 6) {
    for (int i = 0; i < query_count; i++) {
      total_found += rangeQueryBTreeStyle(
          query_ids[i], query_ids[i] + range_size, use_binary_search);
    }
  } else if (mode == 7) {
    for (int i = 0; i < query_count; i++) {
      total_found += rangeQueryBPlusStyle(
          query_ids[i], query_ids[i] + range_size, use_linear_search);
    }
  } else if (mode == 8) {
    for (int i = 0; i < query_count; i++) {
      total_found += rangeQueryBPlusStyle(
          query_ids[i], query_ids[i] + range_size, use_binary_search);
    }
  }

  auto end_time = chrono::high_resolution_clock::now();
  auto duration =
      chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count();
  switch (mode) {
    case 1:
      cout << "B-Tree Style  (Linear Search): ";
      break;
    case 2:
      cout << "B-Tree Style  (Binary Search): ";
      break;
    case 3:
      cout << "B+ Tree Style (Linear Search): ";
      break;
    case 4:
      cout << "B+ Tree Style (Binary Search): ";
      break;
    case 5:
      cout << "B-Tree Style  (Linear Search): ";
      break;
    case 6:
      cout << "B-Tree Style  (Binary Search): ";
      break;
    case 7:
      cout << "B+ Tree Style (Linear Search): ";
      break;
    case 8:
      cout << "B+ Tree Style (Binary Search): ";
      break;
  }
  cout << duration / query_count << " ns | " << disk_read_count / query_count
       << " reads\n";
}

void runBenchmark(int N, const int query_ids[]) {
  cout << "\n========== BENCHMARK WITH query_count = " << query_count
       << " ==========\n";
  cout << "N = " << N << '\n';

  int range_size = (N < 2000) ? min_range_size : max_range_size;

  // TODO for students: Run 100 queries (using query_ids) for each of the 8
  // scenarios. Calculate and print the average Execution Time (ns) and Disk
  // Reads. Hint: start_id for Range Query is query_ids[j], and end_id is
  // query_ids[j] + range_size.

  cout << "Point Query Performance Comparison:\n";
  for (int mode = 1; mode <= 4; mode++) {
    runBenchmarkMode(mode, query_ids, range_size);
  }

  cout << '\n';
  cout << "Range Query Performance Comparison:\n";
  for (int mode = 5; mode <= 8; mode++) {
    runBenchmarkMode(mode, query_ids, range_size);
  }
  cout << "\n\n";
}

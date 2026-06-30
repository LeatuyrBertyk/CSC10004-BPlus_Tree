#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Benchmark/benchmark.h"
#include "PageManipulation/Insert/insert.h"
#include "bplus_tree_disk.h"
#include "global_variables.h"

using namespace std;

FILE *db_file = nullptr;
DBHeader header;
int disk_read_count = 0;
int disk_write_count = 0;
int query_count = 100;
int min_range_size = 500;
int max_range_size = 1000;

int main() {
  // Preload the first 100 IDs from dataset_large.csv to serve as benchmark
  // target IDs
  int query_ids[query_count];
  ifstream csv_setup_file("dataset_large.csv");
  if (!csv_setup_file.is_open()) {
    cerr << "Error: Could not find dataset_large.csv. Run data_generator.cpp "
            "first!\n";
    return 1;
  }
  string setup_line;
  getline(csv_setup_file, setup_line); // Skip header row
  int setup_query_count = 0;
  while (setup_query_count < query_count &&
         getline(csv_setup_file, setup_line)) {
    stringstream ss(setup_line);
    string id_str;
    getline(ss, id_str, ',');
    query_ids[setup_query_count++] = stoi(id_str);
  }
  csv_setup_file.close();

  if (setup_query_count < query_count) {
    cerr << "Error: dataset_large.csv does not contain enough records (100 "
            "required) for benchmarking!\n";
    return 1;
  }

  const int DATA_SIZES[] = {1000,   3000,    10000,   30000,   100000,
                            300000, 1000000, 3000000, 10000000};
  const int NUM_SIZES = sizeof(DATA_SIZES) / sizeof(DATA_SIZES[0]);

  for (int i = 0; i < NUM_SIZES; ++i) {
    int N = DATA_SIZES[i];
    string filename = "index_" + to_string(N) + ".dat";

    // TODO for students: Open or initialize the index_N.dat binary DB file at
    // offset 0
    db_file = fopen(filename.c_str(), "r+b");

    if (db_file != nullptr) {
      fseek(db_file, 0, SEEK_SET);
      fread(&header, 1, sizeof(DBHeader), db_file);
    } else {
      db_file = fopen(filename.c_str(), "w+b");

      header.root_offset = -1;
      header.total_nodes = 0;

      fseek(db_file, 0, SEEK_SET);
      fwrite(&header, 1, sizeof(DBHeader), db_file);
    }

    // DATA SIZE WARNING: dataset_large.csv contains 10,000,000 rows
    // (~700MB). Reading and writing this file can take 5 - 15 minutes. It
    // is highly recommended to debug with N <= 1,000,000 first. If the DB
    // is new, insert the first N records from dataset_large.csv
    // TODO for students: Implement the CSV reading and insertion logic for
    // the first N records
    if (header.root_offset == -1) {
      cout << "Importing " << N << " records...\n";
      ifstream csv_file("dataset_large.csv");
      if (!csv_file.is_open()) {
        cerr << "Error: Could not find dataset_large.csv!\n";
        fclose(db_file);
        return 1;
      }

      string line;
      getline(csv_file, line); // Skip header row

      string prefix = "DummyData_Payload_For_ID_";
      int count = 0;
      while (count < N && getline(csv_file, line)) {
        // Parse ID and Payload from the CSV line
        // Call insertRecord(...)

        stringstream ss(line);
        string id_str, payload_str;

        getline(ss, id_str, ',');
        getline(ss, payload_str);

        if (!payload_str.empty() && payload_str.back() == '\r') {
          payload_str.pop_back();
        }
        int id = stoi(id_str);
        if (payload_str.find(prefix) == 0) {
          payload_str = payload_str.substr(prefix.length());
        }

        insertRecord(id, payload_str.c_str());

        count++;
      }
      csv_file.close();
    }

    // Run benchmark on this database file
    runBenchmark(N, query_ids);

    // TODO: Close the file
    fclose(db_file);
    db_file = nullptr;
  }

  return 0;
}

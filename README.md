# DATABASE STORAGE ENGINE WITH ON-DISK B+ TREE

[![Language: C++](https://img.shields.io/badge/Language-C++-blue.svg)](https://isocpp.org/)
[![Platform: Linux](https://img.shields.io/badge/Platform-Ubuntu-orange.svg)](https://ubuntu.com/)

A database storage engine with on-disk **B+ Tree** implementation from scratch. This project serves as a role of Database Systems architecture, focusing on the trade-offs between Disk I/O and CPU-bound search strategies.

## Overview
This storage engine is designed to handle datasets exceeding RAM capacity by using a fixed-size 4KB page structure to interface directly with physical storage. The implementation provides a benchmark between **B-Tree** and **B+ Tree** by analyzing the performance of Point queries and Range queries under special strategies.

## Repository Structure
```plaintext
Solution/
├── main.cpp
├── data_generator.cpp
├── bplus_tree_disk.h
├── global_variables.h
├── Benchmark/
│   ├── benchmark.cpp
│   └── benchmark.h
├── PageManipulation/  # 4 fundamental operations to manage B+ Tree data structure
│   ├── Insert/
│   ├── Query/
│   └── ReadWrite/
│   └── Search/
Report.pdf
```

## Requirements
To compile and run this project, the following environment is recommended:
- **Compiler**: A `C++11` (later) compiler (e.g., `g++` or `clang++`).
- **OS**: Linux-based system (Ubuntu 22.04+ recommended).
- **RAM**: Minimum 16MB for the engine's memory constraints.
- **Storage**: SSD recommended for optimal performance.

## Getting Started
1. Clone the repository:
   ```bash
   git clone https://github.com/LeatuyrBertyk/CSC10004-BPlus_Tree
   ```
2. Generate dataset:
   ```bash
   g++ -O3 data_generator.cpp -o generator.exe  # Generates 'dataset_large.csv'
   ```
3. Compile the engine:
   ```bash
   g++ -03 *.cpp -o engine.exe
   ```
4. Execution:
   ```bash
   .\engine.exe
   ```

## Execution & Benchmarking
After executing program, the console will output results showing the Mean Execution tie (ns) and Mean Disk reads for each configuration. In a specific, the engine will initialize B+ Tree structure from the dataset, run 100 random queries for both Point and Range queries across various data sizes $N$.

Following the empirical measurement, the project requires an exploration about four advanced topics:
- Range Query Performance & Disk I/O Bottleneck
- B+ Tree Height & Node Structure Optimization
- CPU-Bound and I/O-Bound Interaction
- Data Fragmentation & System Reality

Detailed analyses and plots for these topics are documented in the `Report.pdf` file.

## Conclusion
This implementation validates that the B+ Tree architecture is superior for disk-based storage engines. Following more in report file to get more detailed analyses.

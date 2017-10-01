# MySimpleDatabase

[![Build status for c++](https://api.travis-ci.org/mculinovic/MySimpleDatabase.svg?branch=master)](https://travis-ci.org/mculinovic/MySimpleDatabase)

MySimpleDatabase consists of KVStore which is a c++ thread-safe implementation of key-value store database
and its testing environment - KVStoreTests - which implements unit tests, performance tests
and simulations for KVStore.

Limitations - key value store has records of fixed size. Record consists of key(16B), value(100B) and status (4B).
Status can be valid or invalid because KVStore does not erase deleted records immediately, but only
marks them as invalid. Invalid records are erased from database on closing.

## Dependencies

### Linux

Application uses following software:

1. g++ 4.9+
2. cmake 3.2+

## Installation

CmakeLists is provided in the project root folder. By running the following commands:

```bash
git clone https://github.com/mculinovic/MySimpleDatabase
cd MySimpleDatabase
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
Library named will appear `libKVStore.a` in the `build/KVStore/lib` directory.
Also, test executables will appear in `build/KVStoreTests/bin` directory.


## Usage

### Library

Simple library usage can be seen in the following `example.cpp` file. This code shows how to get consensus and multiple sequence alignment for a set of sequences without quality values.

```cpp
#include "KVStore/include/KVStore.h"

int main(int argc, char** argv) {

    KVStore *db;
    KVStore::Open("test.db", &db);
    
    db->Set("key1", "value1");
    db->Set("key2", "value2");
    db->Set("key3", "value3");
    bool success = db->Delete("key1");
    std::cout << db->Get("key2").value_or("Record with key key1 does not exist") << std::endl; // prints value1
    std::cout << db->Get("key1").value_or("Record with key key1 does not exist") << std::endl; // prints string given in value_or
    return 0;
}
```

This code can be compiled from MySimpleDatabase root directory with:
```bash
g++ example.cpp -std=c++14 -IKVStore/include/ -Lbuild/KVStore/lib -lKVStore -Lbuild/KVStore/vendor/glog-0.3.5 -lglog -o example
```

On the other hand, if you are using `cmake` you can add KVStore to your project by adding commands `add_subdirectory(vendor/KVStore EXCLUDE_FROM_ALL)` and `target_link_libraries(your_exe KVStore)` to your main CMakeLists file.

### Tests

Tests should be ran via scripts in MySimpleDatabase/Scripts directory.
Furthermore, because of hardcoded file paths they should be executed from MySimpleDatabase directory.
All tests are executed on a database with 10k records. Maximum number of operations made in each test
and simulation is 1k.

Run unit tests:
```./Scripts/run_unit_tests.sh ```

Script runs all unit tests and writes the database log to ```./unit_tests_logs``` directory.

Run performance tests:
```./Scripts/run_benchmark.sh```

Script runs all performance tests and writes the database log to ```./benchmark_logs``` directory.
Results of tests are written in ```./benhcmark_output/benchmark.json```.

Run simulations:
```./Scripts/run_simulations.sh```
Script runs all simulations and writes the database log to ```./simulation_logs``` directory.
Results of tests are written in ```./simulation_output/simulation.json```.

Results of a test can be compared using the benchmark tool:

```python KVStoreTests/vendor/benchmark/tools/compare_bench.py <benchmark_output1.json> <benchmark_output2.json>```

## Results

Results of benchmarks and simulations for each version can be found in the results directory of the repository. 

Version 0.1 is implemented as a simple file stream input output locked with a ```mutex``` which was manually
locked and unlocked. This version was used as a starting point for the testing environment.

Version 0.2 uses a ```lock_guard``` instead of manual locking/unlocking and on opening caches offset
of each record in file. This way, iterating over a file to search for a record with a key for each operation
was removed. As a result, all operations had better performance.

Version 0.3 tried to use ```shared_timed_mutex``` instead of a basic ```mutex``` and a ```shared_lock```
instead of a ```lock_guard``` for better multithreaded performance of ```KVStore:Get``` operation. Some improvements
in performance can be seen, but they weren't as expected. These results should be looked into more thoroughly.

## Contact information

For additional information, help and bug reports please send an email to: marko.culinovic@gmail.com.

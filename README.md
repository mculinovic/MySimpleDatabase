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
g++ example.cpp -std=c++11 -IKVStore/include/ -Lbuild/KVStore/lib -lKVStore -o example
```

On the other hand, if you are using `cmake` you can add KVStore to your project by adding commands `add_subdirectory(vendor/KVStore EXCLUDE_FROM_ALL)` and `target_link_libraries(your_exe KVStore)` to your main CMakeLists file.

### Tests

Tests should be ran via scripts in MySimpleDatabase/Scripts directory.
Also, because of hardcoded file paths they should be executed from MySimpleDatabase directory.

Unit tests:
```./Scripts/run_unit_tests.sh ```

Performance tests:
```./Scripts/run_benchmark.sh```

Simulations:
```./Scripts/run_simulations.sh```

## Contact information

For additional information, help and bug reports please send an email to: marko.culinovic@gmail.com.

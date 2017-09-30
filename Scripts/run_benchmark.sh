#!/bin/bash

# create temp databases
mkdir tmp
cp ./KVStoreTests/databases/test10k.db tmp/benchmark_read_10k.db
cp ./KVStoreTests/databases/test10k.db tmp/benchmark_write_10k.db
cp ./KVStoreTests/databases/test10k.db tmp/benchmark_delete_10k.db

#set log for database using GLOG environment variable
mkdir benchmark_logs
export GLOG_log_dir="./benchmark_logs"

# create output folder for benchmarks
mkdir benchmark_output

# run benchmark
./build/KVStoreTests/bin/runPerformance --benchmark_out="benchmark_output/benchmark.json" --benchmark_out_format=json

# delete tmp databases
rm -rf tmp


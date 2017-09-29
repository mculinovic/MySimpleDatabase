#!/bin/bash

# create temp databases
mkdir tmp
cp ./KVStoreTests/databases/test10k.db tmp/simulate_read_only_10k.db
cp ./KVStoreTests/databases/test10k.db tmp/simulate_set_10k.db
cp ./KVStoreTests/databases/test10k.db tmp/simulate_get_10k.db

#set log for database using GLOG environment variable
mkdir simulation_logs
GLOG_log_dir="./simulation_logs"

# create output folder for benchmarks
mkdir simulation_output

# run benchmark
./build/KVStoreTests/bin/runPerformance --benchmark_out="simulation_output/simulation.json" --benchmark_out_format=json

# delete tmp databases
rm -rf tmp
#!/bin/bash

#set log for database using GLOG environment variable
mkdir unit_tests_logs
GLOG_log_dir="./unit_tests_logs"

./build/KVStoreTests/bin/runTests
# delete tmp db
rm ./validateGet.db
rm ./validateSet.db
rm ./validateDelete.db
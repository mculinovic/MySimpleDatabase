#!/bin/bash

./build/KVStoreTests/bin/runTests
# delete tmp db
rm ./validateGet.db
rm ./validateSet.db
rm ./validateDelete.db
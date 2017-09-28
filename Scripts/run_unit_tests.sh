#!/bin/bash

./build/KVStoreTests/bin/runTests
# delete tmp db
rm ./build/KVStoreTests/bin/validateGet.db
rm ./build/KVStoreTests/bin/validateSet.db
rm ./build/KVStoreTests/bin/validateDelete.db
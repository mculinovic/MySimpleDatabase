#include <cstdio>
#include <iostream>
#include <thread>
#include <vector>
#include "KVStore/include/KVStore.h"
#include "KVStore/db/KVStoreImpl.h"

static void test(int thread_id) {
    KVStore *db;
    KVStore::Open("test.db", &db);
    if (db == nullptr) {
        std::cout << "Unsucessful db opening" << std::endl;
    }
    std::string key("key");
    key += std::to_string(thread_id);
    db->Set(key, "marko1");
}

std::vector<std::string> constructKeys(int size, std::string &prefix) {
    std::vector<std::string> keys;
    for (int i = 0; i < size; ++i) {
        std::string key(prefix + std::to_string(i));
        keys.emplace_back(key);
    }
    return keys;
}

std::vector<std::string> constructValues(int size, std::string &prefix) {
    std::vector<std::string> values;
    for (int i = 0; i < size; ++i) {
        std::string value(prefix + std::to_string(i));
        values.emplace_back(value);
    }
    return values;
}

int main(int argc, char **argv) {

    KVStore *db1;
    std::string _filename = "test.db";
    KVStore::Open(_filename, &db1, true);
    db1->ReadData();
    db1->Set("key", "test1");
    db1->ReadData();
    int thread_num = 2;
    std::thread threads[thread_num];
    for (int i = 0; i < thread_num; ++i) {
        threads[i] = std::thread(test, i + 1);
    }
    for (auto& th: threads) {
        th.join();
    }
    db1->ReadData();
    db1->Close();

//    std::string test_db = "../TestDatabases/test10k.db";
    KVStore *db;
    KVStore::Open(_filename, &db);
//    KVStore::Open(test_db, &db);
    if (db == nullptr) {
        std::cout << "Unsucessful db opening" << std::endl;
    }

//    int num_records = 10000;
//    std::string empty_prefix;
//    auto keys = constructKeys(num_records, empty_prefix);
//    auto values = constructValues(num_records, empty_prefix);
//    for (int i = 0; i < num_records; ++i) {
//        db->Set(keys[i], values[i]);
//    }
    db->ReadData();
    db->Close();

//    db->Set("key1", "marko1");
//    db->Set("key23", "mrk2");
//    db->Set("key556", "tester");
//
//    db->ReadData();
//
//    std::cout << "Changing existing data..." << std::endl;
//
//    db->Set("key1", "m31");
//    db->Set("key23", "mrk24324");
//    db->Set("key556", "testA");
//
//    db->ReadData();
//
//    std::cout << "Using get to fetch data..." << std::endl;
//    std::cout << db->Get("key1").value_or("test") << std::endl;
//    std::cout << db->Get("key23").value_or("test") << std::endl;
//    std::cout << db->Get("key556").value_or("test") << std::endl;
//    std::cout << db->Get("key224").value_or("test") << std::endl;
//
//    db->Delete("key1");
//    db->Delete("key23");
//    std::cout << "After deleting" << std::endl;
//    db->ReadData();
//
//    db->Set("key1", "m31");
//    db->Set("key23", "mrk24324");
//    std::cout << "After inserting" << std::endl;
//    db->ReadData();
//
//    db->Delete("key1");
//    db->Delete("key23");
//    db->Close();
//
//    std::cout << "Reopened" << std::endl;
//    db->ReadData();
    return 0;
}
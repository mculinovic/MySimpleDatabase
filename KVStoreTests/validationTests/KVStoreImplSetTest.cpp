/**
 * @file KVStoreImplSetTest.cpp
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Unit tests for KVStore::Set functionality
 * @details Implements units tests for KVStore::Set functionality
 * covering situations where:
 * 1) set record with non existing key in db
 * 2) set record with existing key in db
 * 3) set records with non existing keys in db - multithreaded
 * 4) set recorrds with existing keys in db - multihtreaded
 */

#include <gtest/gtest.h>
#include <fstream>
#include <thread>
#include <unordered_set>
#include "../../KVStore/include/KVStore.h"
#include "Utility.h"

using std::ios;

namespace KVStoreValidation {

    /**
     * @brief Unit tests fixture for testing KVStore::Set
     * @details KVStoreImplSetTest fixture creates
     *  new empty database for each unit test
     *  and stores key and values for testing
     */
    class KVStoreImplSetTest : public ::testing::Test {
    protected:

        virtual void SetUp() {
            db_name = "validateSet.db";
            // open db in truncate mode
            KVStore::Open(db_name, &db, true);
            key_buffer = new char[KEY_SIZE];
            value_buffer = new char[VALUE_SIZE];
            status_buffer = new char[RECORD_SIZE];
            thread_num = 4;
            num_records = 1000;
            for (int i = 1; i <= thread_num; ++i) {
                for (int j = 0; j < num_records; ++j) {
                    auto key = std::to_string(i) + ":" + std::to_string(j);
                    valid_keys.emplace(key); // store valid key
                    valid_values.emplace(std::to_string(i)); // store valid value
                }
            }
        }

        virtual void TearDown() {
            db->Close();
        }

        KVStore *db;
        std::string db_name;
        char *key_buffer;
        char *value_buffer;
        char *status_buffer;
        int thread_num;
        int num_records;
        std::unordered_set<std::string> valid_keys;
        std::unordered_set<std::string> valid_values;

    };


    /**
     * @brief Unit test for KVStore::Set adding new record to db
     * @details Test adds new record to database and checks if
     * operation went successful
     */
    TEST_F(KVStoreImplSetTest, SetNew) {
        std::string key("key test");
        std::string value("value test");
        db->Set(key, value);
        std::ifstream ifs(db_name, ios::in | ios::binary);
        ifs.seekg(0, ios::end);
        // validate right amount of bytes are written to db
        ASSERT_EQ(ifs.tellg(), RECORD_SIZE);

        ifs.seekg(0, ios::beg);
        int records_count = 0;
        while (true) {
            Utility::read_record(ifs, &key_buffer, &value_buffer, &status_buffer);
            if (ifs.eof()) break;
            ++records_count;
        }
        // close stream
        ifs.close();

        // validate only one record is written
        ASSERT_EQ(records_count, 1);
        // validate key content
        ASSERT_EQ(key_buffer, key);
        // validate value content
        ASSERT_EQ(value_buffer, value);
        // validate status
        ASSERT_EQ(static_cast<int>(*status_buffer), KVStore::VALID);
    }


    /**
     * @brief Unit test for KVStore::Set for changing values of existing records
     * @details Test first adds new record to database and afterwards
     * tries to change value for that record
     */
    TEST_F(KVStoreImplSetTest, SetExisting) {
        std::string key("key1");
        std::string value("value1");
        // add new record to db
        db->Set(key, value);
        value = "value2";
        // change value of record
        db->Set(key, value);

        std::ifstream ifs(db_name, ios::in | ios::binary);
        ifs.seekg(0, ios::end);
        // validate that file size did not change - only one record written
        ASSERT_EQ(ifs.tellg(), RECORD_SIZE);
        ifs.seekg(0, ios::beg);
        int records_count = 0;
        while (true) {
            Utility::read_record(ifs, &key_buffer, &value_buffer, &status_buffer);
            if (ifs.eof()) break;
            ++records_count;
        }
        // validate only one record is still written
        ASSERT_EQ(records_count, 1);
        // validate key content
        ASSERT_EQ(key_buffer, key);
        // validate value content
        ASSERT_EQ(value_buffer, value);
        // validate status
        ASSERT_EQ(static_cast<int>(*status_buffer), KVStore::VALID);
        // close stream
        ifs.close();
    }


    /**
     * @brief Unit test for KVStore::Set adding new record to db multithreaded
     * @details Test runs multiple threads and all of them try to write new
     * records to database. After all threads finish working, test checks if
     * all records are written to database and all of them have valid keys
     * and valid values
     */
    TEST_F(KVStoreImplSetTest, SetNewMultithread) {
        std::thread threads[thread_num];
        for (int i = 0; i < thread_num; ++i) {
            threads[i] = std::thread(Utility::set_new_data_thread, db, i + 1, num_records);
        }
        for (auto &th: threads) {
            th.join();
        }

        std::ifstream ifs(db_name, ios::in | ios::binary);
        ifs.seekg(0, ios::end);
        // validate that right amount of bytes is written to db
        ASSERT_EQ(ifs.tellg(), RECORD_SIZE * thread_num * num_records);

        ifs.seekg(0, ios::beg);
        int records_count = 0;
        while (true) {
            Utility::read_record(ifs, &key_buffer, &value_buffer, &status_buffer);
            EXPECT_TRUE(valid_keys.find(key_buffer) != valid_keys.end()); // validate key
            EXPECT_TRUE(valid_values.find(value_buffer) != valid_values.end()); // validate value
            EXPECT_EQ(static_cast<int>(*status_buffer), KVStore::VALID); // check status
            if (ifs.eof()) break;
            ++records_count;
        }
        // close stream
        ifs.close();
        // validate records count
        ASSERT_EQ(records_count, thread_num * num_records);
    }


    /**
     * @brief Unit test for KVStore::Set changing values of existing records multithreaded
     * @details Test runs multiple threads and all of them try to write records
     * with same keys and values to database.
     * After all threads finish working, test checks if
     * all records are written to database and all of them have valid keys
     * and valid values
     */
    TEST_F(KVStoreImplSetTest, SetExistingMultithread) {
        std::thread threads[thread_num];
        for (int i = 0; i < thread_num; ++i) {
            threads[i] = std::thread(Utility::set_existing_data_thread, db, i + 1, num_records);
        }
        for (auto &th: threads) {
            th.join();
        }

        std::ifstream ifs(db_name, ios::in | ios::binary);
        ifs.seekg(0, ios::end);
        // validate that num_records are written to database
        ASSERT_EQ(ifs.tellg(), RECORD_SIZE * num_records);

        // create valid keys and valid values
        std::unordered_set<std::string> valid_keys;
        for (int i = 0; i < num_records; ++i) {
            valid_keys.emplace(std::to_string(i));
        }
        std::unordered_set<std::string> valid_values;
        for (int i = 1; i <= thread_num; ++i) {
            valid_values.emplace(std::to_string(i));
        }

        ifs.seekg(0, ios::beg);
        int records_count = 0;
        while (true) {
            Utility::read_record(ifs, &key_buffer, &value_buffer, &status_buffer);
            EXPECT_TRUE(valid_keys.find(key_buffer) != valid_keys.end()); // validate key
            EXPECT_TRUE(valid_values.find(value_buffer) != valid_values.end()); // validate value
            EXPECT_EQ(static_cast<int>(*status_buffer), KVStore::VALID); // validate status
            if (ifs.eof()) break;
            ++records_count;
        }
        // close stream
        ifs.close();
        // validate records count
        ASSERT_EQ(records_count, num_records);
    }


    /**
     * @brief Unit test for KVStore::Open multithreaded
     * @details Test runs multiple threads and all of them try to open same
     * database and write values to it.
     * After all threads finish working, test checks if
     * all records are written to database and all of them have valid keys
     * and valid values
     */
    TEST_F(KVStoreImplSetTest, OpenMultithread) {
        std::thread threads[thread_num];
        for (int i = 0; i < thread_num; ++i) {
            threads[i] = std::thread(Utility::open_db_thread, db_name, i + 1, num_records);
        }
        for (auto &th: threads) {
            th.join();
        }

        std::ifstream ifs(db_name, ios::in | ios::binary);
        ifs.seekg(0, ios::end);
        // validate that all records are written to database
        ASSERT_EQ(ifs.tellg(), RECORD_SIZE * thread_num * num_records);

        ifs.seekg(0, ios::beg);
        int records_count = 0;
        while (true) {
            Utility::read_record(ifs, &key_buffer, &value_buffer, &status_buffer);
            EXPECT_TRUE(valid_keys.find(key_buffer) != valid_keys.end()); // validate key
            EXPECT_TRUE(valid_values.find(value_buffer) != valid_values.end()); // validate value
            EXPECT_EQ(static_cast<int>(*status_buffer), KVStore::VALID); // validate status
            if (ifs.eof()) break;
            ++records_count;
        }
        // validate records count
        ASSERT_EQ(records_count, thread_num * num_records);
        // close stream
        ifs.close();
    }
    
}
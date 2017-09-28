/**
 * @file KVStoreImplDeleteTest.cpp
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Unit tests for KVStore::Delete functionality
 * @details Implements units tests for KVStore::Delete functionality
 * covering situations where:
 * 1) record with key exists and is valid
 * 2) record with key exists and is not valid
 * 3) record does not exist in database
 */

#include <gtest/gtest.h>
#include <fstream>
#include <unordered_set>
#include "../../KVStore/include/KVStore.h"
#include "Utility.h"

using std::ios;

namespace KVStoreValidation {

    /**
     * @brief Unit tests fixture for testing KVStore::Delete
     * @details KVStoreImplDeleteTest fixture creates
     *  new database with 3 records for each unit test
     */
    class KVStoreImplDeleteTest : public ::testing::Test {
    protected:

        virtual void SetUp() {
            db_name = "validateDelete.db";
            // open database with truncate option
            KVStore::Open(db_name, &db, true);
            key_buffer = new char[KEY_SIZE];
            value_buffer = new char[VALUE_SIZE];
            status_buffer = new char[RECORD_SIZE];
            SetupInitialData();
        }

        void SetupInitialData() {
            std::ofstream ofs(db_name, ios::out | ios::trunc | ios::binary);
            if (!ofs.is_open()) {
                std::cout << "Database opening failed" << std::endl;
            }

            // write 10 records to database, 5 valid and 5 invalid
            for (int i = 0; i < num_records; ++i) {
                auto key = "key" + std::to_string(i);
                auto value = "value" + std::to_string(i);
                int status = i < 2 ? KVStore::VALID : KVStore::INVALID;
                Utility::write_record(ofs, key, value, status);
            }

            ofs.close();
        }

        virtual void TearDown() {
            db->Close();
        }

        KVStore *db;
        std::string db_name;
        char *key_buffer;
        char *value_buffer;
        char *status_buffer;
        int num_records = 3;
    };


    /**
     * @brief Unit test for KVStore::Delete when record with key exists and is valid
     * @details Test deletes key and checks if deletion went successful - return value
     * of KVStore::Delete call should be true and status of record in database
     * should be INVALID
     */
    TEST_F(KVStoreImplDeleteTest, DeleteExisting) {
        std::string key = "key1";
        bool ret = db->Delete(key);
        // validate that method went successful
        ASSERT_EQ(ret, true);

        std::ifstream ifs(db_name, ios::in | ios::binary);
        ifs.seekg(0, ios::end);
        // validate right amount of bytes are written to db
        // this should not change with deletion because only status is changed
        // but record still exists
        EXPECT_EQ(ifs.tellg(), RECORD_SIZE * num_records);
        ifs.seekg(0, ios::beg);
        int records_count = 0;
        // read all records from database and count those that have same key as deleted
        // when record with deleted key is detected check if status of this record is INVALID
        while (true) {
            Utility::read_record(ifs, &key_buffer, &value_buffer, &status_buffer);
            if (ifs.eof()) break;
            std::string db_key(key_buffer);
            if (key == db_key) {
                EXPECT_EQ(static_cast<int>(*status_buffer), KVStore::INVALID);
                ++records_count;
            }
        }
        // there can be no records with same key in database
        ASSERT_EQ(records_count, 1);
        ifs.close();
    }


    /**
     * @brief Unit test for KVStore::Delete when record with key does not exist
     * @details Test deletes key and checks if deletion went successful - return value
     * of KVStore::Delete call should be false
     */
    TEST_F(KVStoreImplDeleteTest, DeleteNonExisting) {
        std::string key = "key";
        bool ret = db->Delete(key);
        // check if no deletion operation happened
        ASSERT_EQ(ret, false);

        std::ifstream ifs(db_name, ios::in | ios::binary);
        ifs.seekg(0, ios::end);
        // validate right amount of bytes are written to db
        EXPECT_EQ(ifs.tellg(), RECORD_SIZE * num_records);
        ifs.seekg(0, ios::beg);
        int records_count = 0;
        while (true) {
            Utility::read_record(ifs, &key_buffer, &value_buffer, &status_buffer);
            if (ifs.eof()) break;
            std::string db_key(key_buffer);
            if (key == db_key) {
                ++records_count;
            }
        }
        // validate that there is no record with key in database
        ASSERT_EQ(records_count, 0);
        ifs.close();
    }


    /**
     * @brief Unit test for KVStore::Delete when record with key exists and is invalid
     * @details Test deletes key and checks if deletion went successful - return value
     * of KVStore::Delete call should be false and status of record in database
     * should be INVALID
     */
    TEST_F(KVStoreImplDeleteTest, DeleteAlreadyInvalid) {
        std::string key = "key2";
        bool ret = db->Delete(key);
        // check if no delete operation happened
        ASSERT_EQ(ret, false);

        std::ifstream ifs(db_name, ios::in | ios::binary);
        ifs.seekg(0, ios::end);
        // validate right amount of bytes are written to db
        EXPECT_EQ(ifs.tellg(), RECORD_SIZE * num_records);
        ifs.seekg(0, ios::beg);
        int records_count = 0;
        while (true) {
            Utility::read_record(ifs, &key_buffer, &value_buffer, &status_buffer);
            if (ifs.eof()) break;
            std::string db_key(key_buffer);
            if (key == db_key) {
                // status of record should be INVALID
                EXPECT_EQ(static_cast<int>(*status_buffer), KVStore::INVALID);
                ++records_count;
            }
        }
        // validate that records with given key count is one
        ASSERT_EQ(records_count, 1);
        ifs.close();
    }
}
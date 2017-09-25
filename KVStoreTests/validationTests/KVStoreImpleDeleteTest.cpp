//
// Created by mculinovic on 9/21/2017.
//

#include <gtest/gtest.h>
#include <fstream>
#include <unordered_set>
#include "../../KVStore/include/KVStore.h"
#include "Utility.h"

using std::ios;

namespace KVStoreValidation {

    class KVStoreImplDeleteTest : public ::testing::Test {
    protected:

        virtual void SetUp() {
            db_name = "validateDelete.db";
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


    TEST_F(KVStoreImplDeleteTest, DeleteExisting) {
        std::string key = "key1";
        bool ret = db->Delete(key);
        ASSERT_EQ(ret, true);

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
                EXPECT_EQ(static_cast<int>(*status_buffer), KVStore::INVALID);
                ++records_count;
            }
        }
        ASSERT_EQ(records_count, 1);
        ifs.close();
    }


    TEST_F(KVStoreImplDeleteTest, DeleteNonExisting) {
        std::string key = "key";
        bool ret = db->Delete(key);
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
        ASSERT_EQ(records_count, 0);
        ifs.close();
    }


    TEST_F(KVStoreImplDeleteTest, DeleteAlreadyInvalid) {
        std::string key = "key2";
        bool ret = db->Delete(key);
        ASSERT_EQ(ret, true);

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
                EXPECT_EQ(static_cast<int>(*status_buffer), KVStore::INVALID);
                ++records_count;
            }
        }
        ASSERT_EQ(records_count, 1);
        ifs.close();
    }
}
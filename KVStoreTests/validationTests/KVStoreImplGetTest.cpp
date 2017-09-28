/**
 * @file KVStoreImplGetTest.cpp
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Unit tests for KVStore::Get functionality
 * @details Implements units tests for KVStore::Get functionality
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
     * @brief Unit tests fixture for testing KVStore::Get
     * @details KVStoreImplGetTest fixture creates
     *  new database with 10 records for each unit test
     */
    class KVStoreImplGetTest : public ::testing::Test {
    protected:

        virtual void SetUp() {
            db_name = "validateGet.db";
            // open database with truncate option
            KVStore::Open(db_name, &db, true);
            SetupInitialData();
        }

        void SetupInitialData() {
            std::ofstream ofs(db_name, ios::out | ios::trunc | ios::binary);
            if (!ofs.is_open()) {
                std::cout << "Database opening failed" << std::endl;
            }

            // write 10 records to database, 5 valid and 5 invalid
            for (int i = 0; i < 10; ++i) {
                auto key = "key" + std::to_string(i);
                auto value = "value" + std::to_string(i);
                int status = i < 5 ? KVStore::VALID : KVStore::INVALID;
                Utility::write_record(ofs, key, value, status);
            }

            ofs.close();
        }

        virtual void TearDown() {
            // close database in tear down
            db->Close();
        }

        KVStore *db;
        std::string db_name;
    };


    /**
     * @brief Unit test for KVStore::Get when record with key exists and is valid
     * @details Test reads all five valid values from database and checks
     * if return value exists and is equal to expected value
     */
    TEST_F(KVStoreImplGetTest, GetExisting) {
        for (int i = 0; i < 5; ++i) {
            auto key = "key" + std::to_string(i);
            auto value = "value" + std::to_string(i);
            std::string getValue = db->Get(key).value_or("invalid");
            EXPECT_EQ(getValue, value);
        }
    }


    /**
     * @brief Unit test for KVStore::Get when record does not exist
     * @details Test tries to read non existing record from database
     * and checks if return value is nullopt, i.e. when using
     * value_or("invalid") method of std::experimental::optional checks
     * if returned value is "invalid"
     */
    TEST_F(KVStoreImplGetTest, GetNonExisting) {
        auto key = "key";
        std::string getValue = db->Get(key).value_or("invalid");
        EXPECT_EQ(getValue, "invalid");
    }


    /**
     * @brief Unit test for KVStore::Get when record with key exists and is invalid
     * @details Test reads all five invalid values from database and checks
     * if return value is nullopt, i.e. when using
     * value_or("invalid") method of std::experimental::optional checks
     * if returned value is "invalid"
     */
    TEST_F(KVStoreImplGetTest, GetDeleted) {
        for (int i = 5; i < 10; ++i) {
            auto key = "key" + std::to_string(i);
            auto value = "value" + std::to_string(i);
            std::string getValue = db->Get(key).value_or("invalid");
            EXPECT_EQ(getValue, "invalid");
        }
    }

}
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

    class KVStoreImplGetTest : public ::testing::Test {
    protected:

        virtual void SetUp() {
            db_name = "validateGet.db";
            KVStore::Open(db_name, &db, true);
            SetupInitialData();
        }

        void SetupInitialData() {
            std::ofstream ofs(db_name, ios::out | ios::trunc | ios::binary);
            if (!ofs.is_open()) {
                std::cout << "Database opening failed" << std::endl;
            }

            for (int i = 0; i < 10; ++i) {
                auto key = "key" + std::to_string(i);
                auto value = "value" + std::to_string(i);
                int status = i < 5 ? KVStore::VALID : KVStore::INVALID;
                Utility::write_record(ofs, key, value, status);
            }

            ofs.close();
        }

        virtual void TearDown() {
            db->Close();
        }

        KVStore *db;
        std::string db_name;
    };


    TEST_F(KVStoreImplGetTest, GetExisting) {
        for (int i = 0; i < 5; ++i) {
            auto key = "key" + std::to_string(i);
            auto value = "value" + std::to_string(i);
            std::string getValue = db->Get(key).value_or("invalid");
            EXPECT_EQ(getValue, value);
        }
    }


    TEST_F(KVStoreImplGetTest, GetNonExisting) {
        auto key = "key";
        std::string getValue = db->Get(key).value_or("invalid");
        EXPECT_EQ(getValue, "invalid");
    }


    TEST_F(KVStoreImplGetTest, GetDeleted) {
        for (int i = 5; i < 10; ++i) {
            auto key = "key" + std::to_string(i);
            auto value = "value" + std::to_string(i);
            std::string getValue = db->Get(key).value_or("invalid");
            EXPECT_EQ(getValue, "invalid");
        }
    }

}
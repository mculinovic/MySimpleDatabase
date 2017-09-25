//
// Created by mculinovic on 9/24/2017.
//

#include <fstream>
#include "Utility.h"
#include "../../KVStore/include/KVStore.h"

namespace KVStoreValidation {

    void Utility::write_record(std::ofstream &ofs, std::string &key, std::string &value, int status) {
        ofs.write(key.c_str(), KEY_SIZE);
        ofs.write(value.c_str(), VALUE_SIZE);
        ofs.write(reinterpret_cast<char *>(&status), STATUS_SIZE);
    }

    void Utility::read_record(std::ifstream &ifs, char **key_buffer, char **value_buffer, char **status_buffer) {
        ifs.read(*key_buffer, KEY_SIZE);
        ifs.read(*value_buffer, VALUE_SIZE);
        ifs.read(*status_buffer, STATUS_SIZE);
    }

    void Utility::set_new_data_thread(KVStore *db, int thread_id, int num_records) {
        for (int i = 0; i < num_records; ++i) {
            auto key = std::to_string(thread_id) + ":" + std::to_string(i);
            db->Set(key, std::to_string(thread_id));
        }
    }

    void Utility::open_db_thread(const std::string &db_name, int thread_id, int num_records) {
        KVStore *db;
        KVStore::Open(db_name, &db);
        for (int i = 0; i < num_records; ++i) {
            auto key = std::to_string(thread_id) + ":" + std::to_string(i);
            db->Set(key, std::to_string(thread_id));
        }
    }


    void Utility::set_existing_data_thread(KVStore *db, int thread_id, int num_records) {
        for (int i = 0; i < num_records; ++i) {
            db->Set(std::to_string(i), std::to_string(thread_id));
        }
    }
}
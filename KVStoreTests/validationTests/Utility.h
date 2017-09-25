//
// Created by mculinovic on 9/24/2017.
//

#ifndef KVSTORE_VALIDATION_UTILITY_H
#define KVSTORE_VALIDATION_UTILITY_H


#include "../../KVStore/include/KVStore.h"

namespace KVStoreValidation {

    class Utility {

    public:
        static void write_record(std::ofstream &ofs, std::string &key, std::string &value, int status);

        static void read_record(std::ifstream &ifs, char **key_buffer, char **value_buffer, char **status_buffer);

        static void set_new_data_thread(KVStore *db, int thread_id, int num_records);

        static void open_db_thread(const std::string &db_name, int thread_id, int num_records);

        static void set_existing_data_thread(KVStore *db, int thread_id, int num_records);

    };
}

#endif //KVSTORE_VALIDATION_UTILITY_H

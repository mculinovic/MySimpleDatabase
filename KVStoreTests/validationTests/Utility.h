/**
 * @file Utility.h
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Unit tests utility header file
 * @details Header file with declaration of helper methods
 * for KVStore unit tests
 */

#ifndef KVSTORE_VALIDATION_UTILITY_H
#define KVSTORE_VALIDATION_UTILITY_H


#include "../../KVStore/include/KVStore.h"

namespace KVStoreValidation {

    /**
     * @brief Utility class
     * @details Provides methods writing and reading records. Also provides
     * helper methods for running various threads.
     */
    class Utility {

    public:
        /**
         * @brief Writes record to output stream
         * @details Writes record at the position where the next character is to be inserted into the output stream.
         * @param ofs reference to output stream
         * @param key record key
         * @param value record value
         * @param status record status
         */
        static void write_record(std::ofstream &ofs, std::string &key, std::string &value, int status);

        /**
         * @brief Reads record from input stream
         * @details Reads record at the position of the next character to be extracted from the input stream.
         * @param ofs reference to input stream
         * @param key_buffer buffer to store record key
         * @param value_buffer buffer to store record value
         * @param status_buffer buffer to store record status
         */
        static void read_record(std::ifstream &ifs, char **key_buffer, char **value_buffer, char **status_buffer);

        /**
         * @brief Writes records to database
         * @details Writes num_records to db.
         * Record format:
         * key = thread_id + {1, num_records}
         * value = thread_id
         * @param db database to write records in
         * @param thread_id id of thread
         * @param num_records number of records to write in database
         */
        static void set_new_data_thread(KVStore *db, int thread_id, int num_records);

        /**
         * @brief Writes records to database
         * @details Writes num_records to db.
         * Record format:
         * key = {1, num_records}
         * value = thread_id
         * @param db database to write records in
         * @param thread_id id of thread
         * @param num_records number of records to write in database
         */
        static void set_existing_data_thread(KVStore *db, int thread_id, int num_records);

        /**
         * @brief Opens database and writes records in it
         * @details Opens database in file "db_name" and writes num_records to db.
         * Record format:
         * key = thread_id + {1, num_records}
         * value = thread_id
         * @param db_name filename of database
         * @param thread_id id of thread
         * @param num_records number of records to write in database
         */
        static void open_db_thread(const std::string &db_name, int thread_id, int num_records);

    };
}

#endif //KVSTORE_VALIDATION_UTILITY_H

/**
 * @file KVStore.h
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Public interface for KVStore library
 */

#ifndef KVSTORE_KVSTORE_H
#define KVSTORE_KVSTORE_H

#include <cstdlib>
#include <string>
#include <experimental/optional>

// One record in datatbase consists of key, value and status
// Defined values are in bytes
#define KEY_SIZE 16
#define VALUE_SIZE 100
#define STATUS_SIZE 4
#define RECORD_SIZE (KEY_SIZE + VALUE_SIZE + STATUS_SIZE)


class KVStore {

public:

    KVStore() = default;

    virtual ~KVStore() = default;

    /**
     * @brief Method used for database opening
     * @details Method opens and creates(if it doesn't exist) database with given filename.
     * If database exists and truncate parameter is true then all content of database is deleted.
     * Pointer to database is initialized.
     * @param filename database name
     * @param db pointer to database
     * @param truncate if true all previous content of db is deleted
     */
    static void Open(const std::string& filename, KVStore **db, bool truncate = false);

    /**
     * @brief Close database
     * @details Method removes all records marked as invalid from database. Note: pointer
     * to database can be used afterwards
     */
    virtual void Close() = 0;

    /**
     * @brief Writes record with given key and value to database
     * @details If given key does not exist in database new record is written, if key
     * exists its value is changed to the one given as param
     * @param key record key
     * @param value record value
     */
    virtual void Set(const std::string &key, const std::string &value) = 0;

    /**
     * @brief Reads record with given key from database
     * @details Finds record with given key and reads its value
     * @param key record key
     * @return if record is found and valid then returns records value, otherwise nullopt is return
     */
    virtual std::experimental::optional<std::string> Get(const std::string &key) = 0;

    /**
     * @brief Reads record with given key from database
     * @details Method marks record as invalid, does not erase it from file.
     * @key record key
     * @return true if record is found and marked is invalid, false otherwise
     */
    virtual bool Delete(const std::string &key) = 0;

    // enum for record status
    enum RecordStatus {
        INVALID, // record deleted
        VALID // record isn't deleted
    };

};


#endif //KVSTORE_KVSTORE_H

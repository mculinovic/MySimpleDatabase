//
// Created by mculinovic on 9/19/2017.
//

#ifndef KVSTORE_KVSTORE_H
#define KVSTORE_KVSTORE_H

#include <cstdlib>
#include <string>
#include <experimental/optional>

#define KEY_SIZE 16
#define VALUE_SIZE 100
#define STATUS_SIZE 4
#define RECORD_SIZE (KEY_SIZE + VALUE_SIZE + STATUS_SIZE)


class KVStore {

public:

    KVStore() = default;

    virtual ~KVStore() = default;

    static void Open(const std::string& filename, KVStore **db, bool truncate = false);

    virtual void Close() = 0;

    virtual void Set(const std::string &key, const std::string &value) = 0;

    virtual std::experimental::optional<std::string> Get(const std::string &key) = 0;

    virtual bool Delete(const std::string &key) = 0;

    virtual void ReadData() = 0;

    enum RecordStatus {
        INVALID,
        VALID
    };

};


#endif //KVSTORE_KVSTORE_H

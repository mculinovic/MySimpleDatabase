//
// Created by mculinovic on 9/19/2017.
//

#ifndef KVSTORE_KVSTOREIMPL_H
#define KVSTORE_KVSTOREIMPL_H

#include "../include/KVStore.h"

#include <string>
#include <mutex>

class KVStoreImpl: public KVStore {
public:
    KVStoreImpl() = default;
    explicit KVStoreImpl(const std::string& filename);

    ~KVStoreImpl() override = default;

    // Implementations of KVStore interface
    void Set(const std::string &key, const std::string &value) override;
    std::experimental::optional<std::string> Get(const std::string &key) override;
    bool Delete(const std::string &key) override;
    void Close() override;
    void ReadData() override;

private:
    friend class KVStore;
    std::mutex _mutex;
    std::string _filename;
    bool _isLoggerInitialized;

    int OpenDB(const std::string& filename, bool truncate);

    static KVStoreImpl& Instance() {
        static KVStoreImpl instance;
        return instance;
    }
};

struct KVStoreRecord {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    char status[STATUS_SIZE];
};


#endif //KVSTORE_KVSTOREIMPL_H

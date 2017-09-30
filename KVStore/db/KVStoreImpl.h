/**
 * @file KVStoreImpl.h
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 */

#ifndef KVSTORE_KVSTOREIMPL_H
#define KVSTORE_KVSTOREIMPL_H

#include "../include/KVStore.h"

#include <string>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>

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

private:
    friend class KVStore;
    std::shared_timed_mutex _mutex;
    std::string _filename;
    bool _isLoggerInitialized;
    // key, offset map  - offset is position of record in file if looked from file beginning
    std::unordered_map<std::string, int> _offset;

    // tries to open database
    int OpenDB(const std::string& filename, bool truncate);
    // reads whole database and memorizes records offsets from beginning of file
    void ReadData();

    // local static singleton - thread safe
    static KVStoreImpl& Instance() {
        static KVStoreImpl instance;
        return instance;
    }
};

// helper struct for reading database record
struct KVStoreRecord {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    char status[STATUS_SIZE];
};


#endif //KVSTORE_KVSTOREIMPL_H

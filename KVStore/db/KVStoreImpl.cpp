//
// Created by mculinovic on 9/19/2017.
//

#include <iostream>
#include <fstream>
#include "KVStoreImpl.h"
#include <glog/logging.h>

using std::ios;

KVStoreImpl::KVStoreImpl(const std::string &filename) {
    _filename = filename;
}


void KVStore::Open(const std::string& filename, KVStore **db, bool truncate) {
    auto& impl = KVStoreImpl::Instance();
    if (impl.OpenDB(filename, truncate)) {
        *db = &impl;
    }
}


void KVStoreImpl::Set(const std::string &key, const std::string &value) {
    _mutex.lock();
    std::fstream fs(_filename, ios::in | ios::out | ios::binary);
    if (!fs.is_open()) {
        LOG(ERROR) << "KVStoreImpl::Set - Could not open database";
        return;
    }
    int status = VALID;

    KVStoreRecord record{};
    bool keyExists = false;
    while (true) {
        fs.read(record.key, KEY_SIZE);
        if (fs.eof()) break;

        auto pos = fs.tellg();
        std::string db_key(record.key);
        if (db_key == key) {
            keyExists = true;
            fs.seekp(pos);
            fs.write(value.c_str(), VALUE_SIZE);
            fs.write(reinterpret_cast<const char *>(&status), STATUS_SIZE);
            LOG(INFO) << "KVStoreImpl::Set Existing key (" << key << "," << value << ")";
            break;
        }

        fs.read(record.value, VALUE_SIZE);
        fs.read(record.status, STATUS_SIZE);
        if (fs.eof()) break;
    }

    if (!keyExists) {
        fs.clear();
        fs.seekp(0, ios::end);
        fs.write(key.c_str(), KEY_SIZE);
        fs.write(value.c_str(), VALUE_SIZE);
        fs.write(reinterpret_cast<const char *>(&status), STATUS_SIZE);
        LOG(INFO) << "KVStoreImpl::Set New key (" << key << "," << value << ")";
    }

    fs.close();
    _mutex.unlock();
}


std::experimental::optional<std::string> KVStoreImpl::Get(const std::string &key) {
    _mutex.lock();
    std::ifstream ifs(_filename, ios::in | ios::binary);
    if (!ifs.is_open()) {
        LOG(ERROR) << "KVStoreImpl::Get - Could not open database";
        return std::experimental::nullopt;
    }

    KVStoreRecord record{};
    while (true) {
        ifs.read(record.key, KEY_SIZE);
        ifs.read(record.value, VALUE_SIZE);
        ifs.read(record.status, STATUS_SIZE);
        if (ifs.eof()) break;
        std::string db_key(record.key);
        auto status = static_cast<int>(*record.status);
        if (db_key == key && status == VALID) {
            LOG(INFO) << "KVStoreImpl::Get Record found (" << key << "," << std::string(record.value) << ")";
            ifs.close();
            _mutex.unlock();
            return std::string(record.value);
        }
    }
    LOG(INFO) << "KVStoreImpl::Get Record with key" << key << " not found";
    ifs.close();
    _mutex.unlock();
    return std::experimental::nullopt;
}


bool KVStoreImpl::Delete(const std::string &key) {
    _mutex.lock();
    std::fstream fs(_filename, ios::in | ios::out | ios::binary);
    if (!fs.is_open()) {
        LOG(ERROR) << "KVStoreImpl::Delete - Could not open database";
        return false;
    }

    int status = INVALID;
    KVStoreRecord record{};
    bool isDeleted = false;
    while (true) {
        fs.read(record.key, KEY_SIZE);
        fs.read(record.value, VALUE_SIZE);
        auto pos = fs.tellg();
        fs.read(record.status, STATUS_SIZE);
        if (fs.eof()) break;

        auto status = static_cast<int>(*record.status);
        std::string db_key(record.key);
        if (db_key == key && status == KVStore::VALID) {
            fs.seekp(pos);
            fs.write(reinterpret_cast<const char *>(&status), STATUS_SIZE);
            isDeleted = true;
            LOG(INFO) << "KVStoreImpl::Delete Record deleted (" << key << "," << std::string(record.value) << ")";
            break;
        }
        fs.read(record.status, STATUS_SIZE);
        if (fs.eof()) break;
    }

    fs.close();
    _mutex.unlock();
    return isDeleted;
}


void KVStoreImpl::Close() {
    _mutex.lock();
    std::ifstream ifs(_filename, ios::in | ios::binary);
    if (!ifs.is_open()) {
       LOG(ERROR) << "Closing failed - input database could not be opened";
        return;
    }

    std::ofstream ofs("temp.db", ios::out | ios::trunc | ios::binary);
    if (!ofs.is_open()) {
        LOG(ERROR) << "Closing failed - temp database could not be opened";
        return;
    }

    KVStoreRecord record{};
    while (true) {
        ifs.read(record.key, KEY_SIZE);
        ifs.read(record.value, VALUE_SIZE);
        ifs.read(record.status, STATUS_SIZE);
        if (ifs.eof()) break;
        auto status = static_cast<int>(*record.status);
        if (status == VALID) {
            ofs.write(record.key, KEY_SIZE);
            ofs.write(record.value, VALUE_SIZE);
            ofs.write(record.status, STATUS_SIZE);
        }
    }

    ifs.close();
    ofs.close();

    remove(_filename.c_str());
    rename("temp.db", _filename.c_str());
    _mutex.unlock();
}


void KVStoreImpl::ReadData() {
    std::ifstream ifs(_filename, ios::in | ios::binary);

    if (!ifs.is_open()) {
        LOG(ERROR) << "KVStoreImpl::ReadData Could not open database";
        return;
    }

    KVStoreRecord record{};
    int recordsCount = 0;
    while (true) {
        ifs.read(record.key, KEY_SIZE);
        ifs.read(record.value, VALUE_SIZE);
        ifs.read(record.status, STATUS_SIZE);
        if (ifs.eof()) break;

        ++recordsCount;
        std::string key(record.key);
        std::string value(record.value);
        int status = static_cast<int>(*record.status);
        std::cout << key << " = " << value << " " << status << std::endl;
    }

    std::cout << recordsCount << std::endl;
    ifs.close();
}


int KVStoreImpl::OpenDB(const std::string& filename, bool truncate) {
    _mutex.lock();
    _filename = filename;
    int status = 1;

    std::ofstream ofs;
    if (truncate) {
        ofs.open(_filename, ios::out | ios::trunc | ios::binary);
    } else {
        ofs.open(_filename, ios::out | ios::binary | ios::app);
    }
    if (!ofs.is_open()) {
        LOG(FATAL) << "Database opening failed" << std::endl;
        status = 0;
    }
    ofs.close();

    if (!_isLoggerInitialized) {
        google::InitGoogleLogging("KVStore");
        LOG(INFO) << "Initializing logger";
        _isLoggerInitialized = true;
    }

    LOG(INFO) << "Opened database " << _filename;
    _mutex.unlock();
    return status;
}

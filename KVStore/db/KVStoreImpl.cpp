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
    std::lock_guard<std::shared_timed_mutex> lock(_mutex);
    std::fstream fs(_filename, ios::in | ios::out | ios::binary);
    if (!fs.is_open()) {
        LOG(ERROR) << "KVStoreImpl::Set - Could not open database";
        return;
    }
    int status = VALID;
    if (_offset.find(key) != _offset.end()) {
        fs.seekp(_offset[key]);
        LOG(INFO) << "KVStoreImpl::Set Existing key (" << key << "," << value << ")";
    } else {
        fs.seekp(0, ios::end);
        auto pos = fs.tellp();
        _offset.emplace(key, pos);
        LOG(INFO) << "KVStoreImpl::Set New key (" << key << "," << value << ")";
    }
    fs.write(key.c_str(), KEY_SIZE);
    fs.write(value.c_str(), VALUE_SIZE);
    fs.write(reinterpret_cast<const char *>(&status), STATUS_SIZE);
    fs.close();
}


std::experimental::optional<std::string> KVStoreImpl::Get(const std::string &key) {
    std::shared_lock<std::shared_timed_mutex> lock(_mutex);
    std::ifstream ifs(_filename, ios::in | ios::binary);
    if (!ifs.is_open()) {
        LOG(ERROR) << "KVStoreImpl::Get - Could not open database";
        return std::experimental::nullopt;
    }

    if (_offset.find(key) != _offset.end()) {
        ifs.seekg(_offset[key]);
        KVStoreRecord record{};
        ifs.read(record.key, KEY_SIZE);
        ifs.read(record.value, VALUE_SIZE);
        ifs.read(record.status, STATUS_SIZE);
        std::string db_key(record.key);
        auto status = static_cast<int>(*record.status);
        if (db_key == key && status == VALID) {
            LOG(INFO) << "KVStoreImpl::Get Record found (" << key << "," << std::string(record.value) << ")";
            ifs.close();
            return std::string(record.value);
        }
    }
    ifs.close();
    LOG(INFO) << "KVStoreImpl::Get Record with key" << key << " not found";
    return std::experimental::nullopt;

}


bool KVStoreImpl::Delete(const std::string &key) {
    std::lock_guard<std::shared_timed_mutex> lock(_mutex);
    std::fstream fs(_filename, ios::in | ios::out | ios::binary);
    if (!fs.is_open()) {
        LOG(ERROR) << "KVStoreImpl::Delete - Could not open database";
        return false;
    }

    int status = INVALID;
    if (_offset.find(key) != _offset.end()) {
        fs.seekp(_offset[key]);
        KVStoreRecord record{};
        fs.read(record.key, KEY_SIZE);
        fs.read(record.value, VALUE_SIZE);
        auto pos = fs.tellg();
        fs.read(record.status, STATUS_SIZE);
        auto current_status = static_cast<int>(*record.status);
        if (current_status == KVStore::VALID) {
            fs.seekp(pos);
            fs.write(reinterpret_cast<const char *>(&status), STATUS_SIZE);
            LOG(INFO) << "KVStoreImpl::Delete Record deleted (" << key << "," << std::string(record.value) << ")";
            return true;
        }
    }
    return false;
}


void KVStoreImpl::Close() {
    std::lock_guard<std::shared_timed_mutex> lock(_mutex);
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
}


void KVStoreImpl::ReadData() {
    _offset.clear();
    std::ifstream ifs(_filename, ios::in | ios::binary);

    if (!ifs.is_open()) {
        LOG(ERROR) << "KVStoreImpl::ReadData Could not open database";
        return;
    }

    KVStoreRecord record{};
    while (true) {
        auto pos = ifs.tellg();
        ifs.read(record.key, KEY_SIZE);
        ifs.read(record.value, VALUE_SIZE);
        ifs.read(record.status, STATUS_SIZE);
        if (ifs.eof()) break;

        _offset.emplace(record.key, pos);
    }
    ifs.close();
}


int KVStoreImpl::OpenDB(const std::string& filename, bool truncate) {
    std::lock_guard<std::shared_timed_mutex> lock(_mutex);
    _filename = filename;
    int status = 1;

    std::fstream fs;
    if (truncate) {
        fs.open(_filename, ios::in | ios::out | ios::trunc | ios::binary);
    } else {
        fs.open(_filename, ios::in | ios::out | ios::binary);
    }
    if (!fs.is_open()) {
        LOG(FATAL) << "Database opening failed" << std::endl;
        status = 0;
    }
    fs.close();

    if (!_isLoggerInitialized) {
        google::InitGoogleLogging("KVStore");
        LOG(INFO) << "Initializing logger";
        _isLoggerInitialized = true;
    }

    ReadData();

    LOG(INFO) << "Opened database " << _filename;
    return status;
}

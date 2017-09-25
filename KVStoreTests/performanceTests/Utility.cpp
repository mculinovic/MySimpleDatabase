//
// Created by mculinovic on 9/24/2017.
//

#include "Utility.h"

namespace KVStorePerformance {

    std::vector<std::string> Utility::constructKeys(int size, std::string &prefix) {
        std::vector<std::string> keys;
        for (int i = 0; i < size; ++i) {
            std::string key(prefix + std::to_string(i));
            keys.emplace_back(key);
        }
        return keys;
    }

    std::vector<std::string> Utility::constructValues(int size, std::string &prefix) {
        std::vector<std::string> values;
        for (int i = 0; i < size; ++i) {
            std::string value(prefix + std::to_string(i));
            values.emplace_back(value);
        }
        return values;
    }
}
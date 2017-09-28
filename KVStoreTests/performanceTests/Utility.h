//
// Created by mculinovic on 9/24/2017.
//

#ifndef KVSTORE_PERFORMANCE_UTILITY_H
#define KVSTORE_PERFORMANCE_UTILITY_H

#include <string>
#include <vector>
#include "../../KVStore/include/KVStore.h"

namespace KVStorePerformance {

    class Utility {

    public:
        static std::vector<std::string> constructKeys(int size, std::string& prefix);
        static std::vector<std::string> constructValues(int size, std::string& prefix);
        static void SetRecordsValidInRange(KVStore *db, const std::vector<std::string>& keys, int start, int end);

    };
}


#endif //KVSTORE_UTILITY_H

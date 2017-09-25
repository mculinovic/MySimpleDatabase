//
// Created by mculinovic on 9/24/2017.
//

#ifndef KVSTORE_PERFORMANCE_UTILITY_H
#define KVSTORE_PERFORMANCE_UTILITY_H

#include <string>
#include <vector>

namespace KVStorePerformance {

    class Utility {

    public:
        static std::vector<std::string> constructKeys(int size, std::string& prefix);
        static std::vector<std::string> constructValues(int size, std::string& prefix);

    };
}


#endif //KVSTORE_UTILITY_H

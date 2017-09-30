/**
 * @file Utility.h
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Performance tests utility header file
 * @details Header file with declaration of helper methods
 * for KVStore performance tests
 */

#ifndef KVSTORE_PERFORMANCE_UTILITY_H
#define KVSTORE_PERFORMANCE_UTILITY_H

#include <string>
#include <vector>
#include "../../KVStore/include/KVStore.h"

namespace KVStorePerformance {

    /**
     * @brief Utility class
     * @details Provides methods for constructing keys and values. Also provides
     * helper method for setting records valid.
     */
    class Utility {

    public:
        /**
         * @brief Generates keys
         * @details Generate string keys in format prefix + {0, size - 1}
         * @param size number of keys to construct
         * @param prefix prefix to attach to each key
         * @return vector of constructed keys
         */
        static std::vector<std::string> constructKeys(int size, std::string& prefix);

        /**
         * @brief Generates values
         * @details Generate string values in format prefix + {0, size - 1}
         * @param size number of values to construct
         * @param prefix prefix to attach to each value
         * @return vector of constructed values
         */
        static std::vector<std::string> constructValues(int size, std::string& prefix);

        /**
         * @brief Sets records valid for keys in given range
         * @param db pointer to database
         * @param keys vector of strings
         * @param start start index of range
         * @param end last index of range exclusive
         */
        static void SetRecordsValidInRange(KVStore *db, const std::vector<std::string>& keys, int start, int end);

    };
}


#endif //KVSTORE_UTILITY_H

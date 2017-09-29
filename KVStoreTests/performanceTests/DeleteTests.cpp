/**
 * @file DeleteTests.cpp
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Performance tests for KVStore::Delete functionality
 * @details Implements performance tests for KVStore::Delete functionality
 * Uses Fixture to setup data for each test. Measures performances
 * for sequential deletions, random deletions in single and multi
 * threaded environment.
 */

#include <benchmark/benchmark.h>
#include <iostream>
#include <random>
#include "../../KVStore/include/KVStore.h"
#include "Utility.h"

namespace KVStorePerformance {

    /**
     * @brief Benchmark fixture for testing KVStore::Delete
     */
    class DeleteFixture : public benchmark::Fixture {

    public:
        /**
         * @brief Constructor
         * @details Opens database and prepares keys for deletion
         */
        DeleteFixture() {
            db = nullptr;
            db_name = "./tmp/benchmark_delete_10k.db";
            num_records = 1000;
            std::string empty_prefix;
            keys = Utility::constructKeys(num_records, empty_prefix);
        }

        KVStore *db;
        std::string db_name;
        std::vector<std::string> keys;
        int num_records;
    };


    /**
     * @brief Benchmark method for testing performance of sequential delete
     * @details Method measures time and calculates how many bytes and items
     * can be deleted in second when records are deleted sequentially
     * @param state Benchmark state has data of:
     * 1) number of threads running and thread indices
     * 2) number of records to delete
     */
    BENCHMARK_DEFINE_F(DeleteFixture, BM_KVStoreDeleteSeq)(benchmark::State& state) {
        KVStore::Open(db_name, &db);
        if (state.thread_index == 0) {
            Utility::SetRecordsValidInRange(db, keys, 0, num_records);
        }
        int records_per_thread = state.range(0) / state.threads;
        while (state.KeepRunning()) {
            for (int i = 0; i < records_per_thread; ++i) {
                db->Delete(keys[i + records_per_thread * state.thread_index]);
            }
            state.PauseTiming();
            // don't measure this
            // all records have to be restored to valid so that next iteration runs on
            // same terms
            int start = records_per_thread * state.thread_index;
            Utility::SetRecordsValidInRange(db, keys, start, start + records_per_thread);
            state.ResumeTiming();
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * records_per_thread);
        state.SetItemsProcessed(state.iterations() * records_per_thread);
    }

    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteSeq)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteSeq)->Threads(2)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteSeq)->Threads(4)->RangeMultiplier(10)->Ranges({{100, 1000}});


    /**
     * @brief Benchmark method for testing performance of random delete
     * @details Method measures time and calculates how many bytes and items
     * can be deleted in second when records are deleted randomly
     * Note: method chooses key by random and information if key was already
     * deleted does not have any affect on choosing
     * @param state Benchmark state has data of:
     * 1) number of threads running and thread indices
     * 2) number of records to delete
     */
    BENCHMARK_DEFINE_F(DeleteFixture, BM_KVStoreDeleteRandom)(benchmark::State& state) {
        KVStore::Open(db_name, &db);
        if (state.thread_index == 0) {
            // set all records in database valid
            Utility::SetRecordsValidInRange(db, keys, 0, num_records);
        }
        int records_per_thread = state.range(0) / state.threads;
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        // every thread deletes keys from its own range
        int start = records_per_thread * state.thread_index;
        std::uniform_int_distribution<> dis(start, start + records_per_thread - 1);
        while (state.KeepRunning()) {
            for (int i = 0; i < records_per_thread; ++i) {
                state.PauseTiming();
                int rndIdx = dis(gen); // don't measure time used for generating random index
                state.ResumeTiming();
                db->Delete(keys[rndIdx]);
            }
            state.PauseTiming();
            // don't measure this
            // all records have to be restored to valid so that next iteration runs on
            // same terms
            Utility::SetRecordsValidInRange(db, keys, start, start + records_per_thread);
            state.ResumeTiming();
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * records_per_thread);
        state.SetItemsProcessed(state.iterations() * records_per_thread);
    }

    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteRandom)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteRandom)->Threads(2)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteRandom)->Threads(4)->RangeMultiplier(10)->Ranges({{100, 1000}});
}
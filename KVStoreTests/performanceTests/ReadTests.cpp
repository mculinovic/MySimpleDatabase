/**
 * @file ReadTests.cpp
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Performance tests for KVStore::Get functionality
 * @details Implements performance tests for KVStore::Get functionality
 * Uses Fixture to setup data for each test. Measures performances
 * for sequential reads, random reads in single and multi
 * threaded environment.
 */

#include <benchmark/benchmark.h>
#include <iostream>
#include <random>
#include "../../KVStore/include/KVStore.h"
#include "Utility.h"

namespace KVStorePerformance {

    /**
     * @brief Benchmark fixture for testing KVStore::Get
     */
    class ReadFixture : public benchmark::Fixture {

    public:
        /**
         * @brief Constructor
         * @details Opens database and prepares keys for get operation
         */
        ReadFixture() {
            db = nullptr;
            std::string db_name = "tmp/benchmark_read_10k.db";
            KVStore::Open(db_name, &db);
            int recordsCount = 1000;
            std::string empty_prefix;
            keys = Utility::constructKeys(recordsCount, empty_prefix);
        }

        ~ReadFixture() {
//            db->Close();
        }

        KVStore *db;
        std::vector<std::string> keys;
    };


    /**
     * @brief Benchmark method for testing performance of sequential read
     * @details Method measures time and calculates how many bytes and items
     * can be read in second when records are read sequentially
     * @param state Benchmark state has data of:
     * 1) number of threads running and thread indices
     * 2) number of records for get operation
     */
    BENCHMARK_DEFINE_F(ReadFixture, BM_KVStoreReadSeq)(benchmark::State& state) {
        int records_per_thread = state.range(0) / state.threads;
        while (state.KeepRunning()) {
            for (int i = 0; i < records_per_thread; ++i) {
                db->Get(keys[i + records_per_thread * state.thread_index]);
            }
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * records_per_thread);
        state.SetItemsProcessed(state.iterations() * records_per_thread);
    }

    BENCHMARK_REGISTER_F(ReadFixture, BM_KVStoreReadSeq)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(ReadFixture, BM_KVStoreReadSeq)->Threads(2)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(ReadFixture, BM_KVStoreReadSeq)->Threads(4)->RangeMultiplier(10)->Ranges({{100, 1000}});


    /**
     * @brief Benchmark method for testing performance of random read
     * @details Method measures time and calculates how many bytes and items
     * can be read in second when records are read randomly
     * @param state Benchmark state has data of:
     * 1) number of threads running and thread indices
     * 2) number of records for get operation
     */
    BENCHMARK_DEFINE_F(ReadFixture, BM_KVStoreReadRandom)(benchmark::State& state) {
        int records_per_thread = state.range(0) / state.threads;
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        // every thread reads keys from its own range
        int start = records_per_thread * state.thread_index;
        std::uniform_int_distribution<> dis(start, start + records_per_thread - 1);
        while (state.KeepRunning()) {
            for (int i = 0; i < records_per_thread; ++i) {
                state.PauseTiming();
                int rndIdx = dis(gen);
                state.ResumeTiming();
                db->Get(keys[rndIdx]);
            }
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * records_per_thread);
        state.SetItemsProcessed(state.iterations() * records_per_thread);
    }

    BENCHMARK_REGISTER_F(ReadFixture, BM_KVStoreReadRandom)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(ReadFixture, BM_KVStoreReadRandom)->Threads(2)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(ReadFixture, BM_KVStoreReadRandom)->Threads(4)->RangeMultiplier(10)->Ranges({{100, 1000}});
}

BENCHMARK_MAIN();

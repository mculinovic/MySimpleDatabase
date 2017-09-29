/**
 * @file WriteTests.cpp
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Performance tests for KVStore::Set functionality
 * @details Implements performance tests for KVStore::Set functionality
 * Measures performances for single thread database fill, multithreaded
 * sequential writes and multithreaded random writes
 */

#include <string>
#include <vector>
#include <benchmark/benchmark.h>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <random>
#include "../../KVStore/include/KVStore.h"
#include "Utility.h"

namespace KVStorePerformance {

    /**
     * @brief Benchmark method for testing performance of database fill
     * @details Method measures time and calculates how many bytes and items
     * can be written in second when records are written sequentially.
     * Note: Method is valid only in single threaded mode
     * @param state Benchmark state has data of:
     * 1) number of records to fill database with
     */
    static void BM_KVStoreFill(benchmark::State &state) {
        std::string key_prefix("key");
        std::string value_prefix("value");
        auto keys = Utility::constructKeys(state.range(0), key_prefix);
        auto values = Utility::constructValues(state.range(0), value_prefix);
        while (state.KeepRunning()) {
            state.PauseTiming();
            std::string db_name = "./tmp/benchmark_fill.db";
            KVStore *db;
            KVStore::Open(db_name, &db, true);
            state.ResumeTiming();
            for (int i = 0; i < state.range(0); ++i) {
                db->Set(keys[i], values[i]);
            }
            state.PauseTiming();
            db->Close();
            state.ResumeTiming();
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * state.range(0));
        state.SetItemsProcessed(state.iterations() * state.range(0));
    }

    BENCHMARK(BM_KVStoreFill)->RangeMultiplier(10)->Ranges({{1, 1000}});


    /**
     * @brief Benchmark fixture for testing KVStore::Set
     */
    class WriteFixture : public benchmark::Fixture {

    public:
        /**
         * @brief Constructor
         * @details Opens database and prepares keys and values
         * for set operations
         */
        WriteFixture() {
            db = nullptr;
            db_name = "./tmp/benchmark_write_10k.db";
            int recordsCount = 1000;
            std::string empty_prefix;
            keys = Utility::constructKeys(recordsCount, empty_prefix);
            values = Utility::constructValues(recordsCount, empty_prefix);
        }

        ~WriteFixture() {
//            db->Close();
        }

        KVStore *db;
        std::string db_name;
        std::vector<std::string> keys;
        std::vector<std::string> values;
    };


    /**
     * @brief Benchmark method for testing performance of sequential write
     * @details Method measures time and calculates how many bytes and items
     * can be written in second when records are written sequentially
     * @param state Benchmark state has data of:
     * 1) number of threads running and thread indices
     * 2) number of records for set operation
     */
    BENCHMARK_DEFINE_F(WriteFixture, BM_KVStoreWriteSeq)(benchmark::State& state) {
        KVStore::Open(db_name, &db);
        int records_per_thread = state.range(0) / state.threads;
        int start = records_per_thread * state.thread_index;
        while (state.KeepRunning()) {
            for (int i = start; i < start + records_per_thread; ++i) {
                db->Set(keys[i], values[i]);
            }
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * records_per_thread);
        state.SetItemsProcessed(state.iterations() * records_per_thread);
    }

    BENCHMARK_REGISTER_F(WriteFixture, BM_KVStoreWriteSeq)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(WriteFixture, BM_KVStoreWriteSeq)->Threads(2)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(WriteFixture, BM_KVStoreWriteSeq)->Threads(4)->RangeMultiplier(10)->Ranges({{100, 1000}});


    BENCHMARK_DEFINE_F(WriteFixture, BM_KVStoreWriteRandom)(benchmark::State& state) {
        KVStore::Open(db_name, &db);
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
                db->Set(keys[rndIdx], values[rndIdx]);
            }
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * records_per_thread);
        state.SetItemsProcessed(state.iterations() * records_per_thread);
    }


    BENCHMARK_REGISTER_F(WriteFixture, BM_KVStoreWriteRandom)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(WriteFixture, BM_KVStoreWriteRandom)->Threads(2)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(WriteFixture, BM_KVStoreWriteRandom)->Threads(4)->RangeMultiplier(10)->Ranges({{100, 1000}});
}

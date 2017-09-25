//
// Created by mculinovic on 9/23/2017.
//

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

    static void BM_KVStoreFill(benchmark::State &state) {
        std::string key_prefix("key");
        std::string value_prefix("value");
        auto keys = Utility::constructKeys(state.range(0), key_prefix);
        auto values = Utility::constructValues(state.range(0), value_prefix);
        while (state.KeepRunning()) {
            state.PauseTiming();
            std::string db_name = "benchmarkTestWrite.db";
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


    static void BM_KVStoreOverwrite(benchmark::State &state) {
        std::string db_name = "benchmarkTestWrite.db";
        KVStore *db;
        KVStore::Open(db_name, &db, true);
        std::string key_prefix("key");
        key_prefix = std::to_string(state.thread_index) + key_prefix;
        std::string value_prefix("value");
        int records_per_thread = state.range(0) / state.threads;
        auto keys = Utility::constructKeys(records_per_thread, key_prefix);
        auto values = Utility::constructValues(records_per_thread, value_prefix);
        for (int i = 0; i < records_per_thread; ++i) {
            db->Set(keys[i], values[i]);
        }
        value_prefix = "OWValue";
        values = Utility::constructValues(records_per_thread, value_prefix);

        while (state.KeepRunning()) {
            for (int i = 0; i < records_per_thread; ++i) {
                db->Set(keys[i], values[i]);
            }
        }
        db->Close();

        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * records_per_thread);
        state.SetItemsProcessed(state.iterations() * records_per_thread);
    }

    BENCHMARK(BM_KVStoreOverwrite)->RangeMultiplier(10)->Ranges({{10, 1000}});
    BENCHMARK(BM_KVStoreOverwrite)->Threads({2})->RangeMultiplier(10)->Ranges({{10, 1000}});
    BENCHMARK(BM_KVStoreOverwrite)->Threads({4})->RangeMultiplier(10)->Ranges({{10, 1000}});


    static void BM_KVStoreWriteRandom(benchmark::State &state) {
        std::string db_name = "benchmarkTestWrite.db";
        KVStore *db;
        KVStore::Open(db_name, &db, true);
        std::string key_prefix("key");
        key_prefix = std::to_string(state.thread_index) + key_prefix;
        std::string value_prefix("value");
        int records_per_thread = state.range(0) / state.threads;
        auto keys = Utility::constructKeys(records_per_thread, key_prefix);
        auto values = Utility::constructValues(records_per_thread, value_prefix);

        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, records_per_thread - 1);
        while (state.KeepRunning()) {
            for (int i = 0; i < records_per_thread; ++i) {
                state.PauseTiming();
                int randIdx = dis(gen);
                state.ResumeTiming();
                db->Set(keys[randIdx], values[randIdx]);
            }
        }
        db->Close();

        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * records_per_thread);
        state.SetItemsProcessed(state.iterations() * records_per_thread);
    }

    BENCHMARK(BM_KVStoreWriteRandom)->RangeMultiplier(10)->Ranges({{10, 1000}});
    BENCHMARK(BM_KVStoreWriteRandom)->Threads({2})->RangeMultiplier(10)->Ranges({{10, 1000}});
    BENCHMARK(BM_KVStoreWriteRandom)->Threads({4})->RangeMultiplier(10)->Ranges({{10, 1000}});

}

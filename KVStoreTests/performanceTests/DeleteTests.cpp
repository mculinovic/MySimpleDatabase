//
// Created by mculinovic on 9/24/2017.
//

#include <benchmark/benchmark.h>
#include <iostream>
#include <random>
#include "../../KVStore/include/KVStore.h"
#include "Utility.h"

namespace KVStorePerformance {

    class DeleteFixture : public benchmark::Fixture {

    public:
        DeleteFixture() {
            db = nullptr;
            std::string db_name = "benchmarkTestDelete.db";
            KVStore::Open(db_name, &db);
            int recordsCount = 1000;
            std::string empty_prefix;
            keys = Utility::constructKeys(recordsCount, empty_prefix);
            values = Utility::constructValues(recordsCount, empty_prefix);
        }

        ~DeleteFixture() {
            db->Close();
        }

        KVStore *db;
        std::vector<std::string> keys;
        std::vector<std::string> values;
    };


    BENCHMARK_DEFINE_F(DeleteFixture, BM_KVStoreDeleteSeq)(benchmark::State& state) {
        while (state.KeepRunning()) {
            for (int i = 0; i < state.range(0); ++i) {
                db->Delete(keys[i]);
            }
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * state.range(0));
        state.SetItemsProcessed(state.iterations() * state.range(0));
    }

    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteSeq)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteSeq)->Threads(2)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteSeq)->Threads(4)->RangeMultiplier(10)->Ranges({{100, 1000}});


    BENCHMARK_DEFINE_F(DeleteFixture, BM_KVStoreDeleteRandom)(benchmark::State& state) {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, state.range(0) - 1);
        while (state.KeepRunning()) {
            for (int i = 0; i < state.range(0); ++i) {
                state.PauseTiming();
                int rndIdx = dis(gen);
                state.ResumeTiming();
                db->Delete(keys[rndIdx]);
            }
        }
        state.SetBytesProcessed(state.iterations() * RECORD_SIZE * state.range(0));
        state.SetItemsProcessed(state.iterations() * state.range(0));
    }

    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteRandom)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteRandom)->Threads(2)->RangeMultiplier(10)->Ranges({{100, 1000}});
    BENCHMARK_REGISTER_F(DeleteFixture, BM_KVStoreDeleteRandom)->Threads(4)->RangeMultiplier(10)->Ranges({{100, 1000}});
}
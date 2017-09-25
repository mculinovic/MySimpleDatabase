//
// Created by mculinovic on 9/25/2017.
//
#include <benchmark/benchmark.h>
#include <thread>
#include <random>
#include "../../KVStore/include/KVStore.h"

static void BM_SimulateReadOnlyDatabase(benchmark::State& state) {
    int operationsCount = state.range(0);
    KVStore *db;
    KVStore::Open("simulateGet.db", &db);

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> disTime(100, 1000);
    std::uniform_int_distribution<> disIdx(0, 9999);
    while (state.KeepRunning()) {
        for (int i = 0; i < operationsCount; ++i) {
            // simulate some useful workload
            std::this_thread::sleep_for(std::chrono::milliseconds(disTime(gen)));
            db->Get(std::to_string(disIdx(gen)));
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
    state.SetBytesProcessed(state.iterations() * state.range(0) * RECORD_SIZE);
}

BENCHMARK(BM_SimulateReadOnlyDatabase)->Threads(4)->Args({100})->Repetitions(2);


static void BM_SimulateRealUsageDatabase(benchmark::State& state) {
    int operationsCount = state.range(0);
    KVStore *db;
    KVStore::Open("simulateReal.db", &db);

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> disTime(100, 1000);
    std::uniform_int_distribution<> disIdx(0, 9999);
    std::uniform_int_distribution<> disOperation(0, 9);
    while (state.KeepRunning()) {
        for (int i = 0; i < operationsCount; ++i) {
            // simulate some useful workload
            std::this_thread::sleep_for(std::chrono::milliseconds(disTime(gen)));
            std::string key = std::to_string(disIdx(gen));
            int operation = disOperation(gen);
            if (operation < 2) {
                // 20% chance for delete operation
                db->Delete(key);
            } else if (operation < 5) {
                // 30% chance for set operation
                std::string value = std::to_string(disIdx(gen));
                db->Set(key, value);
            } else {
                // 50% chance for get operation
                db->Get(std::to_string(disIdx(gen)));
            }
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
    state.SetBytesProcessed(state.iterations() * state.range(0) * RECORD_SIZE);
}

BENCHMARK(BM_SimulateRealUsageDatabase)->Threads(4)->Args({100})->Repetitions(2);


static void BM_SimulateRealUsageSetDatabase(benchmark::State& state) {
    int operationsCount = state.range(0);
    KVStore *db;
    KVStore::Open("simulateReal.db", &db);

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> disTime(100, 1000);
    std::uniform_int_distribution<> disIdx(0, 9999);
    std::uniform_int_distribution<> disOperation(0, 9);
    while (state.KeepRunning()) {
        for (int i = 0; i < operationsCount; ++i) {
            // simulate some useful workload
            std::this_thread::sleep_for(std::chrono::milliseconds(disTime(gen)));
            std::string key = std::to_string(disIdx(gen));
            int operation = disOperation(gen);
            if (operation < 2) {
                // 20% chance for delete operation
                db->Delete(key);
            } else if (operation < 5) {
                // 30% chance for set operation
                db->Get(std::to_string(disIdx(gen)));
            } else {
                // 50% chance for set operation
                std::string value = std::to_string(disIdx(gen));
                db->Set(key, value);
            }
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
    state.SetBytesProcessed(state.iterations() * state.range(0) * RECORD_SIZE);
}

BENCHMARK(BM_SimulateRealUsageSetDatabase)->Threads(4)->Args({100})->Repetitions(2);

BENCHMARK_MAIN();


/**
 * @file Simulations.cpp
 * @author Marko Culinovic <marko.culinovic@gmail.com>
 * @brief Simulations for KVStore database
 * @details Implements simulations for different usages of KVStore and benchmarks it.
 * Implemented simulations:
 * 1) Read only database - 100% get operations
 * 2) 50% get, 30% set, 20% delete operations
 * 3) 50% set, 30% get, 20% delete operations
 * Each simulation is ran 3 times with 4 threads and
 * mean of measured values is calculated
 */
#include <benchmark/benchmark.h>
#include <thread>
#include <random>
#include "../../KVStore/include/KVStore.h"


/**
 * @brief Benchmark method for simulating performance of read only database
 * @details Method measures time and calculates how many bytes and items
 * can be processed in second when db is used as read-only
 * @param state Benchmark state has data of:
 * 1) number of threads running and thread indices
 * 2) number of operations
 */
static void BM_SimulateReadOnlyDatabase(benchmark::State& state) {
    int records_per_thread = state.range(0) / state.threads;
    KVStore *db;
    std::string db_path("./tmp/simulate_read_only_10k.db");
    KVStore::Open(db_path, &db);

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> disTime(100, 1000);
    std::uniform_int_distribution<> disIdx(0, 9999);
    while (state.KeepRunning()) {
        for (int i = 0; i < records_per_thread; ++i) {
            // simulate some useful workload
            std::this_thread::sleep_for(std::chrono::milliseconds(disTime(gen)));
            db->Get(std::to_string(disIdx(gen)));
        }
    }
    state.SetItemsProcessed(state.iterations() * records_per_thread);
    state.SetBytesProcessed(state.iterations() * records_per_thread * RECORD_SIZE);
}

BENCHMARK(BM_SimulateReadOnlyDatabase)->Threads(4)->Args({10000})->Repetitions(3);


/**
 * @brief Benchmark method for simulating performance of database when
 * operations executed are mostly read operations
 * @details Method measures time and calculates how many bytes and items
 * can be processed in second when db is used in environment of
 * 50% read operations, 30% write operations, 20% delete operations.
 * @param state Benchmark state has data of:
 * 1) number of threads running and thread indices
 * 2) number of operations
 */
static void BM_SimulateDatabaseRead(benchmark::State& state) {
    int records_per_thread = state.range(0) / state.threads;
    KVStore *db;
    std::string db_path("./tmp/simulate_get_10k.db");
    KVStore::Open(db_path, &db);

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> disTime(100, 1000);
    std::uniform_int_distribution<> disIdx(0, 9999);
    std::uniform_int_distribution<> disOperation(0, 9);
    while (state.KeepRunning()) {
        for (int i = 0; i < records_per_thread; ++i) {
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
    state.SetItemsProcessed(state.iterations() * records_per_thread);
    state.SetBytesProcessed(state.iterations() * records_per_thread * RECORD_SIZE);
}

BENCHMARK(BM_SimulateDatabaseRead)->Threads(4)->Args({10000})->Repetitions(3);


/**
 * @brief Benchmark method for simulating performance of database when
 * operations executed are mostly write operations
 * @details Method measures time and calculates how many bytes and items
 * can be processed in second when db is used in environment of
 * 50% write operations, 30% read operations, 20% delete operations.
 * @param state Benchmark state has data of:
 * 1) number of threads running and thread indices
 * 2) number of operations
 */
static void BM_SimulateDatabaseSet(benchmark::State& state) {
    int records_per_thread = state.range(0) / state.threads;
    KVStore *db;
    std::string db_path("./tmp/simulate_set_10k.db");
    KVStore::Open(db_path, &db);

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> disTime(100, 1000);
    std::uniform_int_distribution<> disIdx(0, 9999);
    std::uniform_int_distribution<> disOperation(0, 9);
    while (state.KeepRunning()) {
        for (int i = 0; i < records_per_thread; ++i) {
            // simulate some useful workload
            std::this_thread::sleep_for(std::chrono::milliseconds(disTime(gen)));
            std::string key = std::to_string(disIdx(gen));
            int operation = disOperation(gen);
            if (operation < 2) {
                // 20% chance for delete operation
                db->Delete(key);
            } else if (operation < 5) {
                // 30% chance for get operation
                db->Get(std::to_string(disIdx(gen)));
            } else {
                // 50% chance for set operation
                std::string value = std::to_string(disIdx(gen));
                db->Set(key, value);
            }
        }
    }
    state.SetItemsProcessed(state.iterations() * records_per_thread);
    state.SetBytesProcessed(state.iterations() * records_per_thread * RECORD_SIZE);
}

BENCHMARK(BM_SimulateDatabaseSet)->Threads(4)->Args({10000})->Repetitions(3);

BENCHMARK_MAIN();


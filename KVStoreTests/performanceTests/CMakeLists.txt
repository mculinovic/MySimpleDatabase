include_directories($(benchmark_SOURCE_DIR)/include $(benchmark_SOURCE_DIR))

set(CMAKE_CXX_STANDARD 14)

add_executable(runPerformance WriteTests.cpp ReadTests.cpp Utility.cpp Utility.h DeleteTests.cpp)
add_executable(runSimulation Simulations.cpp)

target_link_libraries(runPerformance benchmark)
target_link_libraries(runPerformance KVStore)

target_link_libraries(runSimulation benchmark)
target_link_libraries(runSimulation KVStore)
include_directories($(benchmark_SOURCE_DIR)/include $(benchmark_SOURCE_DIR))

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_executable(runPerformance WriteTests.cpp ReadTests.cpp Utility.cpp Utility.h DeleteTests.cpp)

target_link_libraries(runPerformance benchmark)
target_link_libraries(runPerformance KVStore)

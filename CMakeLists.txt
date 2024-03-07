cmake_minimum_required(VERSION 3.12)
project(CoinCollector)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(CoinCollector main.cpp GameRunner.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(CoinCollector Threads::Threads)
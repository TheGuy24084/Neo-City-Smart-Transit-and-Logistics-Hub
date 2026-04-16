cmake_minimum_required(VERSION 3.10)
project(NeoCityGraph)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(neo_city_hub src/main.cpp src/CityGraph.cpp)

# Header files for IDE support
target_sources(neo_city_hub PRIVATE include/CityGraph.h)

cmake_minimum_required(VERSION 3.19.5)
project("Wc3LiquiWhineTool" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

message(STATUS "Qt version: ${Qt5_VERSION}")

file(GLOB DataPuller_SRC
  "DataPuller/*.h"
  "DataPuller/*.cpp"
)

add_executable(DataPuller ${DataPuller_SRC})
target_link_libraries(DataPuller Qt5::Core Qt5::Network)

file(GLOB DataAnalyzer_SRC
  "DataPuller/CsvReader.h"
  "DataPuller/CsvReader.cpp"
  "DataAnalyzer/*.h"
  "DataAnalyzer/*.cpp"
)

add_executable(DataAnalyzer ${DataAnalyzer_SRC})
target_link_libraries(DataAnalyzer Qt5::Core)


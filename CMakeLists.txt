cmake_minimum_required(VERSION 3.12)
project(NetSentinel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find boost
find_package(Boost REQUIRED COMPONENTS system)

# 2. find libpqxx and PostgreSQL через PkgConfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# path to headers file
include_directories(include ${Boost_INCLUDE_DIRS} ${PQXX_INCLUDE_DIRS})

# src
set(SOURCES 
    src/main.cpp 
    src/DatabaseManager.cpp
)

# executaable file
add_executable(${PROJECT_NAME} ${SOURCES})

# linking
target_link_libraries(${PROJECT_NAME} 
    ${Boost_LIBRARIES} 
    ${PQXX_LIBRARIES} 
    pthread
)
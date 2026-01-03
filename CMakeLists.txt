cmake_minimum_required(VERSION 3.12)
project(NetSentinel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

include_directories(include ${Boost_INCLUDE_DIRS} ${PQXX_INCLUDE_DIRS})

set(SOURCES 
    src/main.cpp 
    src/DatabaseManager.cpp
    src/AuditEngine.cpp
    src/CLIHandler.cpp
    src/DiscoveryEngine.cpp
    src/MetricsEngine.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
    ${Boost_LIBRARIES} 
    ${PQXX_LIBRARIES} 
    pthread
)
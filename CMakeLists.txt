cmake_minimum_required(VERSION 3.5)

project(FastBigFileProcessor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
FILE(GLOB_RECURSE SERVER_CPP_HEADERS "include/*.hpp")
FILE(GLOB_RECURSE SERVER_C_HEADERS "include/*.h")
FILE(GLOB_RECURSE SOURCE_FILES "sources/*.cpp")

find_package(Boost REQUIRED COMPONENTS system filesystem iostreams)

add_executable(
    FastBigFileProcessor
    main.cpp
    ${SOURCE_FILES}
    ${SERVER_C_HEADERS}
    ${SERVER_CPP_HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    Boost::system
    Boost::filesystem
    Boost::iostreams)

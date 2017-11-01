cmake_minimum_required(VERSION 3.8)
project(lemondb)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES
        src/db.cpp src/db.h
        src/db_table.cpp src/db_table.h src/db_table_impl.hpp
        src/query_parser.cpp src/query_parser.h
        src/query.cpp src/query.h
        src/query_builders.cpp src/query_builders.h
        src/query_results.cpp src/query_results.h
        src/management_query.cpp src/management_query.h
        src/data_query.cpp src/data_query.h
        src/arithmetics_query.cpp src/arithmetics_query.h
        src/formatter.h src/uexception.h)

add_executable(lemondb src/main.cpp ${SOURCE_FILES})
add_executable(table_test src/main_test_table.cpp ${SOURCE_FILES})


add_subdirectory(benchmark)

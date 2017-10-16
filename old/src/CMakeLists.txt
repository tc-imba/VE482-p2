cmake_minimum_required(VERSION 3.8)
project(src)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES
        db.cpp db.h
        db_table.cpp db_table.h db_table_impl.hpp
        query_parser.cpp query_parser.h
        query.cpp query.h
        query_builders.cpp query_builders.h
        query_results.cpp query_results.h
        management_query.cpp management_query.h
        data_query.cpp data_query.h
        formatter.h uexception.h )

add_executable(src        main.cpp ${SOURCE_FILES} )
add_executable(table_test main_test_table.cpp ${SOURCE_FILES})
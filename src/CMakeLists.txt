cmake_minimum_required(VERSION 3.8)

set(SOURCE_FILES
        db/db.cpp db/db.h
        db/db_table.cpp db/db_table.h db/db_table_impl.hpp
        query_parser.cpp query_parser.h
        query/query.cpp query/query.h
        query/delete_query.cpp query/delete_query.h
        query_builders.cpp query_builders.h
        query_results.cpp query_results.h
        task_results.h
        management_query.cpp management_query.h
        data_query.cpp data_query.h
        arithmetics_query.cpp arithmetics_query.h
        formatter.h uexception.h query/query_base.h query/task.h query/task.cpp)

add_executable(lemondb main.cpp ${SOURCE_FILES})
target_link_libraries(lemondb pthread)
add_executable(table_test test/main_test_table.cpp ${SOURCE_FILES})


cmake_minimum_required(VERSION 2.7)

add_subdirectory(query)
add_definitions(-DDEBUG)

set(SOURCE_FILES
        db/db.cpp db/db.h
        db/db_table.cpp db/db_table.h
        query_parser.cpp query_parser.h
        query_builders.cpp query_builders.h
        query_results.cpp query_results.h
        management_query.cpp management_query.h
        formatter.h uexception.h
        )
add_executable(lemondb main.cpp ${SOURCE_FILES})
target_link_libraries(lemondb lemondb_query_lib pthread)


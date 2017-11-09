cmake_minimum_required(VERSION 2.5)

project(lemondb_query_lib)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES
        query.cpp query.h query_base.h
        task.cpp task.h
        # Management query
        load_table_query.cpp load_table_query.h
        #dump
        #drop
        #truncate
        #copy
        #quit
        # Data query
        delete_query.cpp delete_query.h
        insert_query.cpp insert_query.h
        update_query.cpp update_query.h
        select_query.cpp select_query.h
        duplicate_query.cpp duplicate_query.h
        swap_query.cpp swap_query.h
        count_query.cpp count_query.h
        add_query.cpp add_query.h
        sub_query.cpp sub_query.h
        sum_query.cpp sum_query.h
        min_query.cpp min_query.h
        max_query.cpp max_query.h
        )

add_library(lemondb_query_lib ${SOURCE_FILES})

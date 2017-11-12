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
        delete_query.cpp delete_query.h #ok
        insert_query.cpp insert_query.h #ok
        update_query.cpp update_query.h #ok
        select_query.cpp select_query.h #ok
        duplicate_query.cpp duplicate_query.h
        swap_query.cpp swap_query.h #ok
        count_query.cpp count_query.h #ok
        add_query.cpp add_query.h
        sub_query.cpp sub_query.h
        sum_query.cpp sum_query.h #ok
        min_query.cpp min_query.h #ok
        max_query.cpp max_query.h #ok
        )

add_library(lemondb_query_lib ${SOURCE_FILES})

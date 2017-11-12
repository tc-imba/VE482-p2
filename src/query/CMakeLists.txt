cmake_minimum_required(VERSION 2.5)

project(lemondb_query_lib)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES
        query.cpp query.h query_base.h
        task.cpp task.h
        # Management query
        management/load_table_query.cpp management/load_table_query.h #ok
        #dump
        #drop
        management/truncate_query.cpp management/truncate_query.h
        management/copy_table_query.cpp management/copy_table_query.h
        management/quit_query.cpp management/quit_query.h
        # Data query
        data/delete_query.cpp data/delete_query.h #ok
        data/insert_query.cpp data/insert_query.h #ok
        data/update_query.cpp data/update_query.h #ok
        data/select_query.cpp data/select_query.h #ok
        data/duplicate_query.cpp data/duplicate_query.h #ok
        data/swap_query.cpp data/swap_query.h #ok
        data/count_query.cpp data/count_query.h #ok
        data/add_query.cpp data/add_query.h #ok
        data/sub_query.cpp data/sub_query.h #ok
        data/sum_query.cpp data/sum_query.h #ok
        data/min_query.cpp data/min_query.h #ok
        data/max_query.cpp data/max_query.h #ok
        )

add_library(lemondb_query_lib ${SOURCE_FILES})

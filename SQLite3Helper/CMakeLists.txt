add_library(SQLite3Helper STATIC sqlite3_helper.h sqlite3_helper.cpp)
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(SQLite3Helper PUBLIC ${PARENT_DIR})
target_compile_features(SQLite3Helper PUBLIC cxx_std_20)

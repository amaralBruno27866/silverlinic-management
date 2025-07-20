cmake_minimum_required(VERSION 3.16)
project(SilverClinic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)
include_directories(include/core)
include_directories(include/forms)

# Source files - recursively find all .cpp files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# For Windows, we'll download SQLite3 or use vcpkg
if(WIN32)
    # Try to find SQLite3 using find_package first
    find_package(SQLite3 QUIET)
    
    if(SQLite3_FOUND)
        target_link_libraries(${PROJECT_NAME} SQLite::SQLite3)
        message(STATUS "Found SQLite3: ${SQLite3_LIBRARIES}")
    else()
        # If not found, we'll handle it manually
        message(STATUS "SQLite3 not found via find_package. You can:")
        message(STATUS "1. Install vcpkg and run: vcpkg install sqlite3")
        message(STATUS "2. Or download SQLite3 manually to libs/ folder")
        
        # Check if manually downloaded SQLite3 exists
        if(EXISTS "${CMAKE_SOURCE_DIR}/libs/sqlite3.h")
            target_include_directories(${PROJECT_NAME} PRIVATE libs)
            target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            message(STATUS "Using manually downloaded SQLite3 from libs/")
        else()
            message(STATUS "SQLite3 will be needed later for database functionality.")
            message(STATUS "For now, building without database support.")
        endif()
    endif()
else()
    # For Unix-like systems, use pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SQLITE3 REQUIRED sqlite3)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SQLITE3_LIBRARIES})
    target_compile_options(${PROJECT_NAME} PRIVATE ${SQLITE3_CFLAGS_OTHER})
endif()

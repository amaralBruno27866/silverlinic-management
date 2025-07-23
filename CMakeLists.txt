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

# Exclude main.cpp from the sources for library
set(LIB_SOURCES ${SOURCES})
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Create a library with all sources except main.cpp
add_library(${PROJECT_NAME}_lib ${LIB_SOURCES})

# Create main executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

# Test executables
add_executable(test_core_classes tests/unit/test_core_classes.cpp)
target_link_libraries(test_core_classes ${PROJECT_NAME}_lib)

add_executable(test_family_physician tests/unit/test_family_physician.cpp)
target_link_libraries(test_family_physician ${PROJECT_NAME}_lib)

add_executable(test_database tests/integration/test_database.cpp)
target_link_libraries(test_database ${PROJECT_NAME}_lib)

# Individual unit tests
add_executable(test_datetime tests/unit/test_datetime.cpp)
target_link_libraries(test_datetime ${PROJECT_NAME}_lib)

add_executable(test_address tests/unit/test_address.cpp)
target_link_libraries(test_address ${PROJECT_NAME}_lib)

add_executable(test_assessor tests/unit/test_assessor.cpp)
target_link_libraries(test_assessor ${PROJECT_NAME}_lib)

add_executable(test_utils tests/unit/test_utils.cpp)
target_link_libraries(test_utils ${PROJECT_NAME}_lib)

add_executable(test_emergency_contact tests/unit/test_emergency_contact.cpp)
target_link_libraries(test_emergency_contact ${PROJECT_NAME}_lib)

# For Windows, we'll download SQLite3 or use vcpkg
if(WIN32)
    # Try to find SQLite3 using find_package first
    find_package(SQLite3 QUIET)
    
    if(SQLite3_FOUND)
        target_link_libraries(${PROJECT_NAME}_lib SQLite::SQLite3)
        target_link_libraries(${PROJECT_NAME} SQLite::SQLite3)
        target_link_libraries(test_core_classes SQLite::SQLite3)
        target_link_libraries(test_family_physician SQLite::SQLite3)
        target_link_libraries(test_database SQLite::SQLite3)
        target_link_libraries(test_datetime SQLite::SQLite3)
        target_link_libraries(test_address SQLite::SQLite3)
        target_link_libraries(test_assessor SQLite::SQLite3)
        target_link_libraries(test_utils SQLite::SQLite3)
        target_link_libraries(test_emergency_contact SQLite::SQLite3)
        message(STATUS "Found SQLite3: ${SQLite3_LIBRARIES}")
    else()
        # If not found, we'll handle it manually
        message(STATUS "SQLite3 not found via find_package. You can:")
        message(STATUS "1. Install vcpkg and run: vcpkg install sqlite3")
        message(STATUS "2. Or download SQLite3 manually to libs/ folder")
        
        # Check if manually downloaded SQLite3 exists
        if(EXISTS "${CMAKE_SOURCE_DIR}/libs/sqlite3.h")
            target_include_directories(${PROJECT_NAME}_lib PRIVATE libs)
            target_include_directories(${PROJECT_NAME} PRIVATE libs)
            target_include_directories(test_core_classes PRIVATE libs)
            target_include_directories(test_family_physician PRIVATE libs)
            target_include_directories(test_database PRIVATE libs)
            target_include_directories(test_datetime PRIVATE libs)
            target_include_directories(test_address PRIVATE libs)
            target_include_directories(test_assessor PRIVATE libs)
            target_include_directories(test_utils PRIVATE libs)
            target_include_directories(test_emergency_contact PRIVATE libs)
            target_link_libraries(${PROJECT_NAME}_lib "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(test_core_classes "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(test_family_physician "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(test_database "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(test_datetime "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(test_address "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(test_assessor "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(test_utils "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
            target_link_libraries(test_emergency_contact "${CMAKE_SOURCE_DIR}/libs/sqlite3.lib")
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
    target_include_directories(${PROJECT_NAME}_lib PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(${PROJECT_NAME} PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(test_core_classes PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(test_family_physician PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(test_database PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(test_datetime PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(test_address PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(test_assessor PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(test_utils PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(test_emergency_contact PRIVATE ${SQLITE3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_lib ${SQLITE3_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${SQLITE3_LIBRARIES})
    target_link_libraries(test_core_classes ${SQLITE3_LIBRARIES})
    target_link_libraries(test_family_physician ${SQLITE3_LIBRARIES})
    target_link_libraries(test_database ${SQLITE3_LIBRARIES})
    target_link_libraries(test_datetime ${SQLITE3_LIBRARIES})
    target_link_libraries(test_address ${SQLITE3_LIBRARIES})
    target_link_libraries(test_assessor ${SQLITE3_LIBRARIES})
    target_link_libraries(test_utils ${SQLITE3_LIBRARIES})
    target_link_libraries(test_emergency_contact ${SQLITE3_LIBRARIES})
    target_compile_options(${PROJECT_NAME}_lib PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(${PROJECT_NAME} PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(test_core_classes PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(test_family_physician PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(test_database PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(test_datetime PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(test_address PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(test_assessor PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(test_utils PRIVATE ${SQLITE3_CFLAGS_OTHER})
    target_compile_options(test_emergency_contact PRIVATE ${SQLITE3_CFLAGS_OTHER})
endif()

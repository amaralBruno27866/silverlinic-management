cmake_minimum_required(VERSION 3.16)
project(SilverClinic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific configuration
if(WIN32)
    # Enable static runtime for Windows
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # Add Windows-specific compile definitions
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    add_compile_definitions(NOMINMAX)
    
    # Find SQLite3 (Windows priority: vcpkg, then manual)
    find_package(SQLite3 QUIET)
    if(SQLite3_FOUND)
        set(SQLITE3_LIBRARY SQLite::SQLite3)
        message(STATUS "Found SQLite3 via vcpkg: ${SQLite3_VERSION}")
    else()
        # Try manual installation in libs/
        find_library(SQLITE3_LIBRARY 
            NAMES sqlite3 sqlite3_static
            PATHS ${CMAKE_SOURCE_DIR}/libs
            NO_DEFAULT_PATH)
        find_path(SQLITE3_INCLUDE_DIR sqlite3.h
            PATHS ${CMAKE_SOURCE_DIR}/libs
            NO_DEFAULT_PATH)
        
        if(SQLITE3_LIBRARY AND SQLITE3_INCLUDE_DIR)
            include_directories(${SQLITE3_INCLUDE_DIR})
            message(STATUS "Found SQLite3 manually: ${SQLITE3_LIBRARY}")
        else()
            message(FATAL_ERROR "SQLite3 not found. Please install via vcpkg or place sqlite3.lib/sqlite3.dll in libs/")
        endif()
    endif()
    
    # Find libharu (Windows priority: vcpkg, then manual)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(HPDF QUIET libhpdf)
    endif()
    
    if(NOT HPDF_FOUND)
        find_library(HPDF_LIBRARY 
            NAMES hpdf libhpdf hpdfs
            PATHS ${CMAKE_SOURCE_DIR}/libs
            NO_DEFAULT_PATH)
        find_path(HPDF_INCLUDE_DIR hpdf.h
            PATHS ${CMAKE_SOURCE_DIR}/libs
            NO_DEFAULT_PATH)
        
        if(HPDF_LIBRARY AND HPDF_INCLUDE_DIR)
            include_directories(${HPDF_INCLUDE_DIR})
            message(STATUS "Found libharu manually: ${HPDF_LIBRARY}")
        else()
            message(FATAL_ERROR "libharu not found. Please install via vcpkg or place libhpdf.lib in libs/")
        endif()
    endif()
    
else()
    # Linux/Unix configuration
    find_library(SQLITE3_LIBRARY sqlite3)
    if(NOT SQLITE3_LIBRARY)
        message(FATAL_ERROR "SQLite3 library not found. Install with: sudo apt-get install libsqlite3-dev")
    endif()

    find_library(HPDF_LIBRARY hpdf)
    find_path(HPDF_INCLUDE_DIR hpdf.h)
    if(NOT HPDF_LIBRARY OR NOT HPDF_INCLUDE_DIR)
        message(FATAL_ERROR "libharu (hpdf) library not found. Install with: sudo apt-get install libhpdf-dev")
    endif()
    
    include_directories(${HPDF_INCLUDE_DIR})
endif()

# Include directories
include_directories(include)
include_directories(include/core)
include_directories(include/forms)
include_directories(include/managers)
include_directories(include/utils)
include_directories(${HPDF_INCLUDE_DIR})

# Source files - recursively find all .cpp files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Exclude main.cpp from the sources for library
set(LIB_SOURCES ${SOURCES})
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Create a library with all sources except main.cpp
add_library(${PROJECT_NAME}_lib ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME}_lib ${SQLITE3_LIBRARY} ${HPDF_LIBRARY})

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

add_executable(test_legal_representative tests/unit/test_legal_representative.cpp)
target_link_libraries(test_legal_representative ${PROJECT_NAME}_lib)

add_executable(test_insurance_company tests/unit/test_insurance_company.cpp)
target_link_libraries(test_insurance_company ${PROJECT_NAME}_lib)

add_executable(test_assessor_manager tests/unit/test_assessor_manager.cpp)
target_link_libraries(test_assessor_manager ${PROJECT_NAME}_lib)

add_executable(test_client_manager tests/unit/test_client_manager.cpp)
target_link_libraries(test_client_manager ${PROJECT_NAME}_lib)

add_executable(test_automobile_anxiety_inventory tests/unit/test_automobile_anxiety_inventory.cpp)
target_link_libraries(test_automobile_anxiety_inventory ${PROJECT_NAME}_lib)

add_executable(test_beck_depression_inventory tests/unit/test_beck_depression_inventory.cpp)
target_link_libraries(test_beck_depression_inventory ${PROJECT_NAME}_lib)

add_executable(test_beck_anxiety_inventory tests/unit/test_beck_anxiety_inventory.cpp)
target_link_libraries(test_beck_anxiety_inventory ${PROJECT_NAME}_lib)

add_executable(test_pain_body_map tests/forms/test_PainBodyMap.cpp)
target_link_libraries(test_pain_body_map ${PROJECT_NAME}_lib)

# Example executables
add_executable(assessor_manager_demo examples/assessor_manager_demo.cpp)
target_link_libraries(assessor_manager_demo ${PROJECT_NAME}_lib)

add_executable(beck_anxiety_inventory_demo examples/beck_anxiety_inventory_demo.cpp)
target_link_libraries(beck_anxiety_inventory_demo ${PROJECT_NAME}_lib)

add_executable(pain_body_map_demo examples/pain_body_map_demo.cpp)
target_link_libraries(pain_body_map_demo ${PROJECT_NAME}_lib)

# Link libraries to all targets
target_link_libraries(${PROJECT_NAME}_lib ${SQLITE3_LIBRARY} ${HPDF_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

# Link to all test executables
foreach(test_target 
    test_core_classes test_family_physician test_database test_datetime test_address 
    test_assessor test_utils test_emergency_contact test_legal_representative 
    test_insurance_company test_assessor_manager test_client_manager 
    test_automobile_anxiety_inventory test_beck_depression_inventory test_beck_anxiety_inventory test_pain_body_map)
    target_link_libraries(${test_target} ${PROJECT_NAME}_lib)
endforeach()

# Link to example executables
target_link_libraries(assessor_manager_demo ${PROJECT_NAME}_lib)
target_link_libraries(beck_anxiety_inventory_demo ${PROJECT_NAME}_lib)
target_link_libraries(pain_body_map_demo ${PROJECT_NAME}_lib)

# PDF Test executable
add_executable(test_pdf tests/integration/test_pdf.cpp)
target_link_libraries(test_pdf ${PROJECT_NAME}_lib)

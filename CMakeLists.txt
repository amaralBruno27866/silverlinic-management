cmake_minimum_required(VERSION 3.16)
project(SilverClinic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option: enable strict compile warnings by default
option(BUILD_STRICT_DEFAULT "Enable strict compiler warnings" ON)

if (BUILD_STRICT_DEFAULT)
    if (MSVC)
        add_compile_options(/W4 /permissive-)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

# Position independent code for shared lib friendliness
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

# Installation rules
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Install static assets (HTML templates)
if (EXISTS ${CMAKE_SOURCE_DIR}/web/views)
    install(DIRECTORY web/views DESTINATION ${CMAKE_INSTALL_DATADIR}/silverclinic)
endif()

# Optionally install existing database (mainly for demo). In production a migration/init step should create it.
if (EXISTS ${CMAKE_SOURCE_DIR}/data/clinic.db)
    install(FILES data/clinic.db DESTINATION ${CMAKE_INSTALL_DATADIR}/silverclinic OPTIONAL)
endif()

# Optional test build toggle
option(BUILD_TESTING "Build test executables" ON)

if(BUILD_TESTING)
    # Collect unit test sources under tests/integration/unit
    file(GLOB UNIT_TEST_SOURCES tests/integration/unit/*.cpp)
    foreach(test_src ${UNIT_TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)
        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} ${PROJECT_NAME}_lib)
    endforeach()

    # Form tests (explicit list to ensure inclusion)
    set(FORMS_TEST_SOURCES
        tests/forms/test_PainBodyMap.cpp
        tests/forms/test_ActivitiesOfDailyLiving.cpp
        tests/forms/test_SCL90R.cpp
    )
    foreach(test_src ${FORMS_TEST_SOURCES})
        if(EXISTS ${CMAKE_SOURCE_DIR}/${test_src})
            get_filename_component(test_name ${test_src} NAME_WE)
            add_executable(${test_name} ${test_src})
            target_link_libraries(${test_name} ${PROJECT_NAME}_lib)
        endif()
    endforeach()

    # Higher-level / integration tests
    set(INTEGRATION_TEST_SOURCES
        tests/integration/test_pdf.cpp
        tests/integration/test_database.cpp
        tests/integration/test_datetime_integration.cpp
    tests/integration/test_pragmas.cpp
    )
    foreach(test_src ${INTEGRATION_TEST_SOURCES})
        if(EXISTS ${CMAKE_SOURCE_DIR}/${test_src})
            get_filename_component(test_name ${test_src} NAME_WE)
            add_executable(${test_name} ${test_src})
            target_link_libraries(${test_name} ${PROJECT_NAME}_lib)
        endif()
    endforeach()
endif()

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

# Link to example executables (already linked individually above for clarity)

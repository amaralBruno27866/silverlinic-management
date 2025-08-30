cmake_minimum_required(VERSION 3.16)
project(SilverClinic)

enable_testing()

# Enable AUTOMOC for Qt (required to generate moc files for Q_OBJECT)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option: enable strict compile warnings by default
option(BUILD_STRICT_DEFAULT "Enable strict compiler warnings" ON)
option(BUILD_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(ENABLE_SANITIZERS "Enable address/undefined sanitizers on Debug builds (GCC/Clang)" OFF)

if (BUILD_STRICT_DEFAULT)
    if (MSVC)
        add_compile_options(/W4 /permissive-)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        # Helpful extra warnings (opt-in lightweight)
        add_compile_options(-Wmissing-include-dirs -Wnull-dereference -Wduplicated-cond -Wduplicated-branches -Wlogical-op)
    endif()
endif()

if (BUILD_WARNINGS_AS_ERRORS)
    if (MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

if (ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
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
# (StructuredLogger added under src/utils; glob captures automatically)

# Exclude main.cpp from the sources for library
set(LIB_SOURCES ${SOURCES})
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/gui/main.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/gui/dashboard.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/gui/assessor/assessor_model.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/gui/assessor/assessor_view.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/gui/assessor/assessor_dialog.cpp")

# Create a library with all sources except main.cpp
add_library(${PROJECT_NAME}_lib ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME}_lib ${SQLITE3_LIBRARY} ${HPDF_LIBRARY})

# Create main executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

# Installation rules
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_lib EXPORT SilverClinicTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Install public headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SilverClinic FILES_MATCHING PATTERN "*.h")

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
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()

    # Form tests (explicit list to ensure inclusion)
    set(FORMS_TEST_SOURCES
        tests/forms/test_PainBodyMap.cpp
        tests/forms/test_ActivitiesOfDailyLiving.cpp
        tests/forms/test_SCL90R.cpp
    tests/forms/test_BDIManager.cpp
    tests/forms/test_BAIManager.cpp
    tests/forms/test_SCL90RManager.cpp
    tests/forms/test_AAIManager.cpp
    tests/forms/test_AAIManager_CSVNegative.cpp
    )
    foreach(test_src ${FORMS_TEST_SOURCES})
        if(EXISTS ${CMAKE_SOURCE_DIR}/${test_src})
            get_filename_component(test_name ${test_src} NAME_WE)
            add_executable(${test_name} ${test_src})
            target_link_libraries(${test_name} ${PROJECT_NAME}_lib)
            add_test(NAME ${test_name} COMMAND ${test_name})
        endif()
    endforeach()

    # Higher-level / integration tests
    set(INTEGRATION_TEST_SOURCES
        tests/integration/test_pdf.cpp
        tests/integration/test_database.cpp
        tests/integration/test_datetime_integration.cpp
    tests/integration/test_pragmas.cpp
    tests/integration/test_form_generation.cpp
    )
    # Managers specific tests (new)
    if(EXISTS ${CMAKE_SOURCE_DIR}/tests/managers/test_AddressManager.cpp)
        add_executable(test_AddressManager tests/managers/test_AddressManager.cpp)
        target_link_libraries(test_AddressManager ${PROJECT_NAME}_lib)
        add_test(NAME test_AddressManager COMMAND test_AddressManager)
    endif()
    foreach(test_src ${INTEGRATION_TEST_SOURCES})
        if(EXISTS ${CMAKE_SOURCE_DIR}/${test_src})
            get_filename_component(test_name ${test_src} NAME_WE)
            add_executable(${test_name} ${test_src})
            target_link_libraries(${test_name} ${PROJECT_NAME}_lib)
            add_test(NAME ${test_name} COMMAND ${test_name})
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

# ---- Package Config (basic) ----
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SilverClinicConfigVersion.cmake
    VERSION 0.1.0
    COMPATIBILITY SameMajorVersion)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SilverClinicConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/SilverClinicConfig.cmake @ONLY)

install(EXPORT SilverClinicTargets
        NAMESPACE SilverClinic::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SilverClinic)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SilverClinicConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SilverClinicConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SilverClinic)

# ---- Qt6 GUI Integration ----
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# GUI executable target
# add_executable(silverclinic_gui src/gui/main.cpp src/gui/dashboard.cpp
#     src/gui/assessor/assessor_model.cpp
#     src/gui/assessor/assessor_view.cpp
#     src/gui/assessor/assessor_dialog.cpp
# )
# target_link_libraries(silverclinic_gui PRIVATE SilverClinic_lib Qt6::Widgets)

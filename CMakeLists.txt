cmake_minimum_required(VERSION 3.21)

# TODO: strip objects
# TODO: test application linking to static libaray
# TODO: make test application optional
# TODO: more robust install

project(guardfw
        VERSION 0.0.1
        DESCRIPTION "templated C++ wrapper around posix functions for proper error handling"
        HOMEPAGE_URL "https://simon.gleissner.de"
        LANGUAGES CXX
        )

include(GNUInstallDirs)

find_package(Catch2 3 REQUIRED)

option(COMPILE_COVERAGE "configure gcc for compiling with coverage code" OFF)

execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE SHORT_GIT_HASH_RESULT
        OUTPUT_VARIABLE SHORT_GIT_HASH_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (SHORT_GIT_HASH_RESULT EQUAL 0)
    set(SHORT_GIT_HASH "${SHORT_GIT_HASH_OUTPUT}")
    set(VERSION_ADDENDUM "-git_${SHORT_GIT_HASH_OUTPUT}")
endif ()

set(CMAKE_CXX_STANDARD 23)

add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -O3
        -Wno-terminate
)

if (COMPILE_COVERAGE)
    add_compile_options(-g -O0 --coverage -fprofile-abs-path)
    add_link_options(--coverage)
endif ()

set(public_headers
        src/guardfw/file_descriptor.hpp
        src/guardfw/guard.hpp
        src/guardfw/guard_event.hpp
        src/guardfw/guard_timer.hpp
        src/guardfw/traits.hpp
        src/guardfw/version.hpp
        src/guardfw/wrapped_eventfd.hpp
        src/guardfw/wrapped_fnctl.hpp
        src/guardfw/wrapped_ioctl.hpp
        src/guardfw/wrapped_mqueue.hpp
        src/guardfw/wrapped_signalfd.hpp
        src/guardfw/wrapped_socket.hpp
        src/guardfw/wrapped_timerfd.hpp
        src/guardfw/wrapped_unistd.hpp
        src/guardfw/wrapper.hpp
        ${CMAKE_BINARY_DIR}/guardfw/generated_config.hpp
        )

set(library_sources
        src/guard_event.cpp
        src/guard_timer.cpp
        )

set(test_sources
        src/tests/test_version.cpp
        src/tests/test_context_posix.cpp
        )

add_library(guardfw SHARED
        ${library_sources}
        #        ${public_headers}
        )

add_library(guardfw-static STATIC
        ${library_sources}
        )

add_executable(guardfw-tests
        ${test_sources}
        )

add_dependencies(guardfw-static guardfw-tests)

configure_file(libguardfw.pc.in libguardfw.pc @ONLY)
configure_file(config.hpp.in guardfw/generated_config.hpp @ONLY)
configure_file(sonar-project.properties.in ${CMAKE_SOURCE_DIR}/sonar-project.properties @ONLY)

set_target_properties(guardfw PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER "${public_headers}"
        )

include_directories(
        src                    # for guardfw/*
        ${CMAKE_BINARY_DIR}    # for guardfw/generated_config.h
)

target_link_libraries(guardfw-tests PRIVATE
        Catch2::Catch2WithMain
        guardfw
        -lrt
        -pthread
        )

install(TARGETS guardfw guardfw-static
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/guardfw
        )

install(FILES ${CMAKE_BINARY_DIR}/libguardfw.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )

include(CTest)
include(Catch)
catch_discover_tests(guardfw-tests)

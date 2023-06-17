cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0048 NEW)

# TODO: strip objects
# TODO: test application linking to static libaray
# TODO: make test application optional
# TODO: more robust install

project(guardfw
        VERSION 0.0.3
        DESCRIPTION "Templated C++ wrapper around Linux API functions for proper error handling" # without '.'
        HOMEPAGE_URL "http://guardfw.de"
        LANGUAGES CXX
        )

# TODO perhaps get description from a file, used for rpm spec files
set(project_long_description "${CMAKE_PROJECT_DESCRIPTION}.")

set(github_home "https://github.com/sgleissner")
set(github_project "${github_home}/${CMAKE_PROJECT_NAME}")

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
        -Wconversion
        -Wno-terminate
        -Werror
        -O3
)

if (COMPILE_COVERAGE)
    add_compile_options(-g -O0 --coverage -fprofile-abs-path)
    add_link_options(--coverage)
endif ()

set(public_headers
        src/guardfw/exceptions.hpp
        src/guardfw/file_descriptor.hpp
        src/guardfw/guard.hpp
        src/guardfw/guard_event.hpp
        src/guardfw/guard_file.hpp
        src/guardfw/guard_message.hpp
        src/guardfw/guard_signal.hpp
        src/guardfw/guard_timer.hpp
        src/guardfw/traits.hpp
        src/guardfw/version.hpp
        src/guardfw/wrapped_eventfd.hpp
        src/guardfw/wrapped_fcntl.hpp
        src/guardfw/wrapped_ioctl.hpp
        src/guardfw/wrapped_mqueue.hpp
        src/guardfw/wrapped_resource.hpp
        src/guardfw/wrapped_signal.hpp
        src/guardfw/wrapped_signalfd.hpp
        src/guardfw/wrapped_socket.hpp
        src/guardfw/wrapped_timerfd.hpp
        src/guardfw/wrapped_unistd.hpp
        src/guardfw/wrapper.hpp
        ${CMAKE_BINARY_DIR}/guardfw/generated_config.hpp
        )

set(library_sources
        src/exceptions.cpp
        src/guard.cpp
        src/guard_event.cpp
        src/guard_file.cpp
        src/guard_message.cpp
        src/guard_signal.cpp
        src/guard_timer.cpp
        src/guard_timer_constant.cpp
        )

set(test_sources
        src/tests/test_version.cpp
        src/tests/test_wrapper.cpp
        src/tests/test_file.cpp
        )

add_library(guardfw SHARED
        ${library_sources}
        )

add_library(guardfw-static STATIC
        ${library_sources}
        )

add_executable(guardfw-tests
        ${test_sources}
        )

add_dependencies(guardfw-tests guardfw-static)

set(file_generated_warning "This is a cmake-generated file, do not edit!")

configure_file(cmake-templates/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
configure_file(cmake-templates/config.hpp.in ${PROJECT_NAME}/generated_config.hpp @ONLY)
file(REMOVE ${CMAKE_SOURCE_DIR}/sonar-project.properties)
configure_file(cmake-templates/sonar-project.properties.in ${CMAKE_SOURCE_DIR}/sonar-project.properties @ONLY)
configure_file(cmake-templates/obs-rpm.spec.in ${CMAKE_SOURCE_DIR}/openbuildservice/obs-rpm.spec @ONLY)

set_target_properties(guardfw PROPERTIES
        VERSION ${PROJECT_VERSION}
        NO_SONAME ON
        # SOVERSION ${PROJECT_VERSION}
        PUBLIC_HEADER "${public_headers}"
        OUTPUT_NAME ${PROJECT_NAME}-${PROJECT_VERSION}
        )

set_target_properties(guardfw-static PROPERTIES
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME ${PROJECT_NAME}
        )

include_directories(BEFORE SYSTEM   # higher priority than older installed library
        src
        ${CMAKE_BINARY_DIR}    # for generated_config.h
        )

target_link_libraries(guardfw-tests PRIVATE
        Catch2::Catch2WithMain
        guardfw
        -lrt
        -pthread
        )

install(TARGETS guardfw
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        # RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        )

install(TARGETS guardfw-static
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        # RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        # PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        )

# install(TARGETS guardfw-tests
#        # LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#        # PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
#        )

install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig
        )

include(CTest)
include(Catch)
catch_discover_tests(guardfw-tests)

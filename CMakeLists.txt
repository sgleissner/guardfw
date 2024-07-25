cmake_minimum_required(VERSION 3.28) # v3.28 supports C++20 modules

# force a compiler with enhanced C++ module support

set(CMAKE_C_COMPILER /usr/bin/gcc-14 CACHE PATH "" FORCE)
set(CMAKE_CXX_COMPILER /usr/bin/g++-14 CACHE PATH "" FORCE)

# project settings

project(guardfw        # = CMAKE_PROJECT_NAME
        VERSION 0.0.5
        DESCRIPTION "Templated C++ wrapper around Linux API functions for proper error handling" # without trailing '.'
        HOMEPAGE_URL "http://guardfw.de"
        LANGUAGES CXX
)

# TODO perhaps get description from a file, used for rpm spec files
set(project_long_description "${CMAKE_PROJECT_DESCRIPTION}.") # with trailing '.'

set(github_home "https://github.com/sgleissner")
set(github_project "${github_home}/${CMAKE_PROJECT_NAME}")

# compiler settings
set(CMAKE_CXX_STANDARD 23)

add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wno-terminate # allows destructors to throw and abort the application
        -Werror
        -Wno-error=deprecated-declarations # using attribute [[deprecated]] is not an error
        -O3
        -fmodules-ts # modules are currently not explicitly enabled in gcc with -std=c++20
)

# module source files
set(module_sources
        modules/guardfw.cppm
        modules/exceptions.cppm
        modules/file_descriptor.cppm
        modules/traits.cppm
        modules/wrapper.cppm
        modules/wrappers/wrapped_epoll.cppm
        modules/wrappers/wrapped_eventfd.cppm
        modules/wrappers/wrapped_fcntl.cppm
        modules/wrappers/wrapped_io_uring.cppm
        modules/wrappers/wrapped_ioctl.cppm
        modules/wrappers/wrapped_mman.cppm
        modules/wrappers/wrapped_mqueue.cppm
        modules/wrappers/wrapped_resource.cppm
        modules/wrappers/wrapped_signal.cppm
        modules/wrappers/wrapped_signalfd.cppm
        modules/wrappers/wrapped_socket.cppm
        modules/wrappers/wrapped_stdio.cppm
        modules/wrappers/wrapped_timerfd.cppm
        modules/wrappers/wrapped_timerfd_constant.cppm
        modules/wrappers/wrapped_unistd.cppm
        ${CMAKE_BINARY_DIR}/generated/config.cppm
)

# unit test files
set(test_sources
        tests/test_config.cpp
        tests/test_wrapper.cpp
        tests/test_wrapped_io_uring.cpp
        tests/test_wrapped_mman.cpp
)

include(GNUInstallDirs)                          # predefined Filesystem Hierachy Standard folders
include(cmake-includes/generic-setup.cmake)      # generic project setup & helpers
include(cmake-includes/generic-sonarqube.cmake)  # generic SonarQube support
include(cmake-includes/generic-library.cmake)    # generic c++ module-based library
include(cmake-includes/generic-tests.cmake)      # generic Catch2 unit tests

# small example application

add_executable(example)

target_sources(example
        PRIVATE
        src/example.cpp
)

target_link_libraries(example PRIVATE
        ${target_modules}
        -lrt
        -pthread
)

add_dependencies(guardfw-tests example)

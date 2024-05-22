# compile Catch2 tests

option(COMPILE_TESTS "build Catch2 module tests" ON)
if (COMPILE_TESTS)
    set(target_tests ${CMAKE_PROJECT_NAME}-tests)

    # use Catch2 as test framework
    find_package(Catch2 3 REQUIRED)

    add_executable(${target_tests})

    target_sources(${target_tests}
            PRIVATE
            ${test_sources}
    )

    target_link_libraries(${target_tests} PRIVATE
            Catch2::Catch2WithMain
            ${target_modules}
            -lrt
            -pthread
    )

    include(CTest)
    include(Catch)
    catch_discover_tests(${target_tests})
endif ()

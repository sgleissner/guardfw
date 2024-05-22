# generic C++-Modules-based library support

set(target_modules ${CMAKE_PROJECT_NAME}-modules)
set(modules_fileset ${CMAKE_PROJECT_NAME}_fileset)
set(install_cppmodules_dir "src/c++-modules/${CMAKE_PROJECT_NAME}")           # for .cppm files
set(install_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}")  # for .cmake files

# generating files
configure_file(cmake-templates/config.cppm.in generated/config.cppm @ONLY)
#configure_file(cmake-templates/obs-rpm.spec.in ${CMAKE_SOURCE_DIR}/openbuildservice/obs-rpm.spec @ONLY)


# build and install C++-module library

add_library(${target_modules})

target_sources(${target_modules} PUBLIC
        FILE_SET ${modules_fileset}
        TYPE CXX_MODULES
        FILES ${module_sources}
)

install(FILES ${module_sources}
        DESTINATION ${install_cppmodules_dir}
)


# generate and install cmake files

set(install_modules "")
foreach (module_source ${module_sources})
    cmake_path(GET module_source FILENAME install_module)
    list(APPEND install_modules ${install_module})
endforeach ()

set(install_target ${cmake_prj_namespace}::${CMAKE_PROJECT_NAME})

include(CMakePackageConfigHelpers)

configure_file(cmake-templates/project.cmake.in generated/${CMAKE_PROJECT_NAME}.cmake @ONLY)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})   # standard include folder, not used, but needed for a check
set(SYSCONFIG_INSTALL_DIR ${CMAKE_INSTALL_SYSCONFDIR}) # standard etc folder, not used, but needed for a check

configure_package_config_file(cmake-templates/project-config.cmake.in
        ${CMAKE_BINARY_DIR}/generated/${CMAKE_PROJECT_NAME}-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
        PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR
)

write_basic_package_version_file(
        ${CMAKE_BINARY_DIR}/generated/${CMAKE_PROJECT_NAME}-config-version.cmake
        VERSION ${CMAKE_PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(FILES ${CMAKE_BINARY_DIR}/generated/${CMAKE_PROJECT_NAME}.cmake
        ${CMAKE_BINARY_DIR}/generated/${CMAKE_PROJECT_NAME}-config.cmake
        ${CMAKE_BINARY_DIR}/generated/${CMAKE_PROJECT_NAME}-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
)

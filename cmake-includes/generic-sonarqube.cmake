# generic SonarQube support

# optional SonarQube support
option(SONARQUBE "generate configuration for SonarQube" OFF)
if (SONARQUBE)
    # add code coverage
    add_compile_options(-g -O0 --coverage -fprofile-abs-path)
    add_link_options(--coverage)

    file(REMOVE ${CMAKE_SOURCE_DIR}/sonar-project.properties)
    configure_file(cmake-templates/sonar-project.properties.in ${CMAKE_SOURCE_DIR}/sonar-project.properties @ONLY)
endif ()

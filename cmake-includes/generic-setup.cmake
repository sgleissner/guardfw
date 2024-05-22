# generic cmake setup

# cmake namespace for target_link_libraries
string(TOUPPER "${CMAKE_PROJECT_NAME}" cmake_prj_namespace)

# warning for .in template generator
set(file_generated_warning "This is a cmake-generated file, do not edit!")

# get short git hash
execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_SHORT_HASH_RESULT
        OUTPUT_VARIABLE GIT_SHORT_HASH_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# get latest tag without additional changes
execute_process(
        COMMAND git describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_TAG_RESULT
        OUTPUT_VARIABLE GIT_TAG_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# get latest tag with additional changes
execute_process(
        COMMAND git describe --tags
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_TAG_CHANGES_RESULT
        OUTPUT_VARIABLE GIT_TAG_CHANGES_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# get git dirty status
execute_process(
        COMMAND git status --porcelain=v1
        COMMAND wc -l
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_DIRTY_RESULT
        OUTPUT_VARIABLE GIT_DIRTY_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# check latest tag, compare git tag with version number
# TODO: to be deleted later
#if ((GIT_TAG_RESULT EQUAL 0) AND (GIT_TAG_CHANGES_RESULT EQUAL 0))
#    if(GIT_TAG_OUTPUT EQUAL PROJECT_VERSION)
#        message("git tag version match")
#    else()
#        message("git tag version mismatch!")
#    endif()
#    if(GIT_TAG_OUTPUT EQUAL GIT_TAG_CHANGES_OUTPUT)
#        message("git tag has no additional changes")
#    else()
#        message("git tag has additional changes!")
#    endif()
#else ()
#    message("git tag unset!")
#endif()

# generate VERSION_ADDENDUM
# TODO: eventually remove messages
if (GIT_SHORT_HASH_RESULT EQUAL 0)
    # decide, if hash shall be appended
    if ((NOT GIT_TAG_RESULT EQUAL 0) OR (NOT GIT_TAG_OUTPUT EQUAL GIT_TAG_CHANGES_OUTPUT))
        # report short git hash
        set(VERSION_ADDENDUM "-git${GIT_SHORT_HASH_OUTPUT}")
        # report additional uncommitted changes
        if ((GIT_DIRTY_RESULT EQUAL 0) AND (NOT GIT_DIRTY_OUTPUT EQUAL 0))
            set(VERSION_ADDENDUM "${VERSION_ADDENDUM}.DIRTY")
        endif ()
        # report git tag version mismatch compared to project version
        if ((NOT GIT_TAG_RESULT EQUAL 0) OR (NOT GIT_TAG_OUTPUT EQUAL PROJECT_VERSION))
            set(VERSION_ADDENDUM "${VERSION_ADDENDUM}.TAGDIRTY")
        endif ()
        # git version not correctly tagged, shall not be released
        set(RELEASABLE_GIT false)
        # debug output
        if (NOT GIT_TAG_RESULT EQUAL 0)
            message("git tag unset!") # checked
        else ()
            message("git tag version unclean")
        endif ()
    else ()
        # correctly tagged, may be released
        set(VERSION_ADDENDUM "")
        set(RELEASABLE_GIT true)
        message("git tag version clean")
    endif ()
else ()
    # no git, can not be released
    set(VERSION_ADDENDUM "")
    set(RELEASABLE_GIT false)
    message("git not used")
endif ()

message("${VERSION_ADDENDUM}")  # TODO: to be removed

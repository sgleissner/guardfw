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

# generate VERSION_ADDENDUM
# TODO: eventually remove messages
if (GIT_SHORT_HASH_RESULT EQUAL 0)
    # decide, if hash shall be appended
    if ((NOT GIT_TAG_RESULT EQUAL 0) OR (NOT GIT_TAG_OUTPUT STREQUAL GIT_TAG_CHANGES_OUTPUT))
        # Additional commits after latest tagged commit, report short git hash
        string(TOUPPER "${GIT_SHORT_HASH_OUTPUT}" short_hash_uppercase)
        set(VERSION_ADDENDUM "-git${short_hash_uppercase}")
        # Additional uncommitted changes, report ".DIRTY"
        if ((GIT_DIRTY_RESULT EQUAL 0) AND (NOT GIT_DIRTY_OUTPUT EQUAL 0))
            set(VERSION_ADDENDUM "${VERSION_ADDENDUM}.DIRTY")
        endif ()
        # git tag != cmake version, report ".TAGDIRTY"
        if ((NOT GIT_TAG_RESULT EQUAL 0) OR
        ((NOT GIT_TAG_OUTPUT STREQUAL PROJECT_VERSION) AND
        (NOT GIT_TAG_OUTPUT STREQUAL "v${PROJECT_VERSION}") AND
        (NOT GIT_TAG_OUTPUT STREQUAL "V${PROJECT_VERSION}")))
            set(VERSION_ADDENDUM "${VERSION_ADDENDUM}.TAGDIRTY")
        endif ()
        # git version not correctly tagged, shall not be released
        set(RELEASABLE_GIT false)
        # debug output
        if (NOT GIT_TAG_RESULT EQUAL 0)
            message("git tag unset!") # checked   # TODO: to be removed
        else ()
            message("git tag version unclean") # checked   # TODO: to be removed
        endif ()
    else ()
        if ((GIT_DIRTY_RESULT EQUAL 0) AND (NOT GIT_DIRTY_OUTPUT EQUAL 0))
            # correctly tagged, but uncommitted changes
            set(VERSION_ADDENDUM "${VERSION_ADDENDUM}.DIRTY")
            set(RELEASABLE_GIT false)
            message("git tag version clean, but uncommitted changes") # checked   # TODO: to be removed
        else ()
            # correctly tagged, may be released
            set(VERSION_ADDENDUM "")
            set(RELEASABLE_GIT true)
            message("git tag version clean")  # TODO: to be removed
        endif ()
    endif ()
else ()
    # no git, can not be released
    set(VERSION_ADDENDUM "")
    set(RELEASABLE_GIT false)
    message("git not used")  # TODO: to be removed
endif ()

message("VERSION_ADDENDUM=${VERSION_ADDENDUM}")  # TODO: to be removed
message("RELEASABLE_GIT=${RELEASABLE_GIT}")

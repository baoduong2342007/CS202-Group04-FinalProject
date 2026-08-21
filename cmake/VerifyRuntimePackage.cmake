# Verify the configured runtime inventory against both source and package roots.
#
# This script intentionally scans only the package's assets/ and levels/
# subtrees.  Executables, DLLs, and CMake metadata are deployment outputs, not
# runtime inventory entries; every file below these two roots must be declared.

foreach(_required_variable IN ITEMS PACKAGE_ROOT SOURCE_ROOT EXPECTED_INVENTORY)
    if(NOT DEFINED ${_required_variable} OR
       "${${_required_variable}}" STREQUAL "")
        message(FATAL_ERROR
            "VerifyRuntimePackage.cmake requires -D${_required_variable}=...")
    endif()
endforeach()

if(NOT EXISTS "${EXPECTED_INVENTORY}" OR IS_DIRECTORY "${EXPECTED_INVENTORY}")
    message(FATAL_ERROR
        "Expected runtime inventory is missing or is not a file: "
        "${EXPECTED_INVENTORY}")
endif()
if(NOT EXISTS "${PACKAGE_ROOT}" OR NOT IS_DIRECTORY "${PACKAGE_ROOT}")
    message(FATAL_ERROR
        "Runtime package root is missing or is not a directory: "
        "${PACKAGE_ROOT}")
endif()
if(NOT EXISTS "${SOURCE_ROOT}" OR NOT IS_DIRECTORY "${SOURCE_ROOT}")
    message(FATAL_ERROR
        "Runtime source root is missing or is not a directory: "
        "${SOURCE_ROOT}")
endif()

file(STRINGS "${EXPECTED_INVENTORY}" _expected_lines)
set(_expected_files)
foreach(_line IN LISTS _expected_lines)
    string(STRIP "${_line}" _relative_path)
    if("${_relative_path}" STREQUAL "")
        continue()
    endif()
    if("${_relative_path}" MATCHES "^[\\/]|(^|/)\\.\\.(/|$)" OR
       "${_relative_path}" MATCHES "[\\\\]" OR
       "${_relative_path}" MATCHES ";")
        message(FATAL_ERROR
            "Unsafe path in expected runtime inventory: ${_relative_path}")
    endif()
    list(FIND _expected_files "${_relative_path}" _duplicate_index)
    if(NOT _duplicate_index EQUAL -1)
        message(FATAL_ERROR
            "Duplicate path in expected runtime inventory: ${_relative_path}")
    endif()
    list(APPEND _expected_files "${_relative_path}")

    set(_source_file "${SOURCE_ROOT}/${_relative_path}")
    if(NOT EXISTS "${_source_file}" OR IS_DIRECTORY "${_source_file}")
        message(FATAL_ERROR
            "Expected runtime source file is missing: ${_source_file}")
    endif()
    set(_package_file "${PACKAGE_ROOT}/${_relative_path}")
    if(NOT EXISTS "${_package_file}" OR IS_DIRECTORY "${_package_file}")
        list(APPEND _missing_files "${_relative_path}")
    endif()
endforeach()

if(NOT DEFINED _missing_files)
    set(_missing_files)
endif()

set(_actual_files)
foreach(_package_subtree IN ITEMS assets levels)
    set(_package_directory "${PACKAGE_ROOT}/${_package_subtree}")
    if(EXISTS "${_package_directory}" AND IS_DIRECTORY "${_package_directory}")
        file(GLOB_RECURSE _subtree_files
             LIST_DIRECTORIES false
             RELATIVE "${PACKAGE_ROOT}"
             "${_package_directory}/*")
        list(APPEND _actual_files ${_subtree_files})
    endif()
endforeach()
list(SORT _expected_files)
list(SORT _actual_files)

set(_unexpected_files)
foreach(_actual_file IN LISTS _actual_files)
    list(FIND _expected_files "${_actual_file}" _expected_index)
    if(_expected_index EQUAL -1)
        list(APPEND _unexpected_files "${_actual_file}")
    endif()
endforeach()
foreach(_expected_file IN LISTS _expected_files)
    list(FIND _actual_files "${_expected_file}" _actual_index)
    if(_actual_index EQUAL -1)
        list(FIND _missing_files "${_expected_file}" _missing_index)
        if(_missing_index EQUAL -1)
            list(APPEND _missing_files "${_expected_file}")
        endif()
    endif()
endforeach()

if(_missing_files OR _unexpected_files)
    set(_diagnostic "Runtime package inventory mismatch.")
    if(_missing_files)
        list(SORT _missing_files)
        string(APPEND _diagnostic "\nMissing files:")
        foreach(_missing_file IN LISTS _missing_files)
            string(APPEND _diagnostic "\n  ${_missing_file}")
        endforeach()
    endif()
    if(_unexpected_files)
        list(SORT _unexpected_files)
        string(APPEND _diagnostic "\nUnexpected files:")
        foreach(_unexpected_file IN LISTS _unexpected_files)
            string(APPEND _diagnostic "\n  ${_unexpected_file}")
        endforeach()
    endif()
    message(FATAL_ERROR "${_diagnostic}")
endif()

message(STATUS
    "Runtime package inventory verified: ${PACKAGE_ROOT} "
    "(${_expected_files})")

cmake_minimum_required(VERSION 3.16)
project(SuperMario VERSION 1.0 LANGUAGES CXX)

# ==============================================================================
# 1. COMPILER & C++ STANDARD CONFIGURATIONS
# ==============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Automatically generate compile_commands.json for IDEs (Clangd/VS Code)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug build type if none is specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# ==============================================================================
# 2. FIND OR DOWNLOAD SFML LIBRARY (cross-platform: Windows, macOS, Linux)
# ==============================================================================
# On Windows: If pre-built SFML is missing in thirdparty/, automatically download and extract it.
# S6-TV1-27: TLS verification is ON, and an existing SFML folder is never deleted
# (a failed configure must not destroy data). Archive hash verification is documented
# below but intentionally not hard-coded until the upstream hash is confirmed.
if(WIN32 AND NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML/lib/cmake/SFML/SFMLConfig.cmake")
    if(MSVC)
        message(STATUS "SFML not found in thirdparty/SFML. Downloading pre-built SFML 3.0.0 MSVC (Visual Studio 2022) (~30MB)...")
        set(SFML_URL "https://github.com/SFML/SFML/releases/download/3.0.0/SFML-3.0.0-windows-vc17-64-bit.zip")
    else()
        message(STATUS "SFML not found in thirdparty/SFML. Downloading pre-built SFML 3.0.0 MinGW-w64 (~30MB)...")
        set(SFML_URL "https://github.com/SFML/SFML/releases/download/3.0.0/SFML-3.0.0-windows-gcc-14.2.0-mingw-64-bit.zip")
    endif()
    set(SFML_ZIP_PATH "${CMAKE_BINARY_DIR}/sfml-3.0.0-win.zip")
    # NOTE: TLS verification is ON. To also verify the archive integrity, set
    # EXPECTED_HASH to the real SHA256 of the downloaded zip (e.g.
    # "SHA256=<hex>"). It is intentionally omitted here because the exact
    # upstream hash must be confirmed by the maintainer before hard-coding.
    file(DOWNLOAD "${SFML_URL}"
                  "${SFML_ZIP_PATH}"
                  TLS_VERIFY ON
                  SHOW_PROGRESS STATUS DOWNLOAD_STATUS)
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(STATUS_CODE EQUAL 0)
        message(STATUS "Extracting SFML 3.0.0 into thirdparty/...")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${SFML_ZIP_PATH}"
                        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/thirdparty")
        # Rename extracted folder (e.g. SFML-3.0.0) to SFML if necessary
        file(GLOB EXTRACTED_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SFML-*")
        if(EXTRACTED_DIR)
            list(GET EXTRACTED_DIR 0 FIRST_DIR)
            file(RENAME "${FIRST_DIR}" "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
        endif()
        file(REMOVE "${SFML_ZIP_PATH}")
        message(STATUS "Pre-built SFML 3.0.0 installed successfully in thirdparty/SFML")
    else()
        list(GET DOWNLOAD_STATUS 1 ERROR_MSG)
        message(FATAL_ERROR "Failed to download pre-built SFML: ${ERROR_MSG}")
    endif()
endif()

# Prioritize local SFML in the `thirdparty/SFML` directory (Windows only)
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
    if(MSVC AND EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML/lib/libsfml-system.a")
        message(FATAL_ERROR
            "thirdparty/SFML contains MinGW libraries, but this configure is using MSVC. "
            "Regenerate with: cmake -S . -B build -G \"MinGW Makefiles\"")
    endif()
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
endif()

# Find SFML 3.0 with required components
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)

# ==============================================================================
# 2.2. FIND OR DOWNLOAD BOX2D LIBRARY
# ==============================================================================
include(FetchContent)
FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG v2.4.1
)
# Disable Box2D unit tests, testbed and documentation to speed up compilation
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE INTERNAL "")
set(BOX2D_BUILD_TESTBED OFF CACHE INTERNAL "")
set(BOX2D_BUILD_DOCS OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(box2d)

# ==============================================================================
# 3. DEFINE TARGET & SOURCE FILES
# ==============================================================================
# Recursively scan all .cpp source files in the src/ directory
# CONFIGURE_DEPENDS ensures CMake automatically detects newly created source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# S6-TV1-26: exclude demo files from the production target so the demo's
# main() does not collide with src/main.cpp (duplicate symbol at link time).
list(FILTER SOURCES EXCLUDE REGEX "src/demo/.*\\.cpp$")

add_executable(main ${SOURCES})

# ==============================================================================
# 4. TARGET BUILD CONFIGURATIONS
# ==============================================================================
# Include directories
target_include_directories(main PRIVATE include)

# Define DEBUG macro for debug builds
target_compile_definitions(main PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_options(main PRIVATE -Wall -Wextra -pedantic)
elseif(MSVC)
    target_compile_options(main PRIVATE /W4 /permissive- /FS)
endif()

# Link SFML and Box2D libraries
target_link_libraries(main PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    box2d
)

include(CTest)
if(BUILD_TESTING)
    add_executable(input_state_tests
        tests/InputStateTests.cpp
        src/patterns/InputHandler.cpp
        src/patterns/InputState.cpp
    )
    target_include_directories(input_state_tests PRIVATE include)
    target_link_libraries(input_state_tests PRIVATE SFML::Window)
    add_test(NAME input_state_tests COMMAND input_state_tests)

    add_executable(tile_collision_span_tests
        tests/TileCollisionSpanTests.cpp
        src/level/TileCollisionSpans.cpp
        src/physics/TileContactResolver.cpp
    )
    target_include_directories(tile_collision_span_tests PRIVATE include)
    target_link_libraries(tile_collision_span_tests PRIVATE box2d)
    add_test(NAME tile_collision_span_tests COMMAND tile_collision_span_tests)

    add_executable(mario_physics_tests
        tests/MarioPhysicsTests.cpp
        src/core/AnimationSystem.cpp
        src/core/TextureManager.cpp
        src/entities/Character.cpp
        src/entities/Entity.cpp
        src/entities/Mario.cpp
        src/patterns/EventBus.cpp
    )
    target_include_directories(mario_physics_tests PRIVATE include)
    target_link_libraries(mario_physics_tests PRIVATE SFML::Graphics box2d)
    add_test(NAME mario_physics_tests COMMAND mario_physics_tests)

    add_executable(play_state_tests
        tests/PlayStateTests.cpp
        src/patterns/EventBus.cpp
    )
    target_include_directories(play_state_tests PRIVATE include)
    target_link_libraries(play_state_tests PRIVATE SFML::Graphics)
    add_test(NAME play_state_tests COMMAND play_state_tests)
endif()

# Platform-specific Release build settings
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(WIN32)
        set_property(TARGET main PROPERTY WIN32_EXECUTABLE TRUE)
    elseif(APPLE)
        set_property(TARGET main PROPERTY MACOSX_BUNDLE TRUE)
    endif()
endif()

# ==============================================================================
# 5. POST-BUILD ACTIONS
# ==============================================================================

# 5.1. Auto-sync assets and levels directories to the build folder.
# S6-TV1-29: `cmake -E copy_directory` only overwrites files whose content/timestamp
# changed, so unchanged assets are not re-copied on every invocation (incremental).
add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/levels" "${CMAKE_BINARY_DIR}/levels"
    COMMENT "Auto-syncing assets and level maps to build directory (incremental)..."
)

# 5.2. Copy SFML DLL files (Windows only)
if(WIN32)
    # Determine the directory containing DLL files
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML/bin")
        set(SFML_DLL_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SFML/bin")
    else()
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(SFML_DLL_DIR "${MINGW_BIN_DIR}")
    endif()

    # List of SFML DLL files to copy (supporting both Release and Debug builds)
    set(SFML_DLL_NAMES
        "sfml-graphics-3.dll"   "sfml-graphics-d-3.dll"
        "sfml-window-3.dll"     "sfml-window-d-3.dll"
        "sfml-system-3.dll"     "sfml-system-d-3.dll"
        "sfml-audio-3.dll"      "sfml-audio-d-3.dll"
    )

    foreach(DLL_NAME IN LISTS SFML_DLL_NAMES)
        set(DLL_PATH "${SFML_DLL_DIR}/${DLL_NAME}")
        if(EXISTS "${DLL_PATH}")
            add_custom_command(TARGET main POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL_PATH}" "$<TARGET_FILE_DIR:main>/"
                COMMENT "Copying ${DLL_NAME} to build output folder..."
            )
        endif()
    endforeach()
endif()

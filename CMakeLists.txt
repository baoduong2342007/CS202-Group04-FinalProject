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
# 2. FIND SFML LIBRARY
# ==============================================================================
# Prioritize local SFML in the `thirdparty/SFML` directory
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
endif()

# Find SFML 3.0 with required components
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)

# ==============================================================================
# 3. DEFINE TARGET & SOURCE FILES
# ==============================================================================
# Recursively scan all .cpp source files in the src/ directory
# CONFIGURE_DEPENDS ensures CMake automatically detects newly created source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(main ${SOURCES})

# ==============================================================================
# 4. TARGET BUILD CONFIGURATIONS
# ==============================================================================
# Include directories
target_include_directories(main PRIVATE include)

# Compiler flags
target_compile_options(main PRIVATE -Wall -Wextra -pedantic)

# Link SFML libraries
target_link_libraries(main PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# Hide console window in Release build type on Windows
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET main PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# ==============================================================================
# 5. POST-BUILD ACTIONS
# ==============================================================================

# 5.1. Copy assets and levels directories to the executable output folder
foreach(DIR "assets" "levels")
    if(EXISTS "${CMAKE_SOURCE_DIR}/${DIR}")
        add_custom_command(TARGET main POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${DIR}" "$<TARGET_FILE_DIR:main>/${DIR}"
            COMMENT "Copying directory '${DIR}' to build output folder..."
        )
    endif()
endforeach()

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

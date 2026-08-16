cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
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
# S6-TV1-27: TLS verification is ON, official archive hashes are pinned below,
# and an existing SFML folder is never deleted by a failed configure.
if(WIN32 AND NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML/lib/cmake/SFML/SFMLConfig.cmake")
    if(MSVC)
        message(STATUS "SFML not found in thirdparty/SFML. Downloading pre-built SFML 3.0.0 MSVC (Visual Studio 2022) (~30MB)...")
        set(SFML_URL "https://github.com/SFML/SFML/releases/download/3.0.0/SFML-3.0.0-windows-vc17-64-bit.zip")
        # S6-TV1-27: verified SHA256 of the official SFML 3.0.0 VC17 64-bit zip.
        set(EXPECTED_SFML_HASH "SHA256=01295AA4E87AEBCD917A4EA67C319F325BE131B83E0F3A85F290F0CE51A492A8")
    else()
        message(STATUS "SFML not found in thirdparty/SFML. Downloading pre-built SFML 3.0.0 MinGW-w64 (~30MB)...")
        set(SFML_URL "https://github.com/SFML/SFML/releases/download/3.0.0/SFML-3.0.0-windows-gcc-14.2.0-mingw-64-bit.zip")
        # S6-TV1-27: verified SHA256 of the official SFML 3.0.0 MinGW-w64 (gcc 14.2.0) 64-bit zip.
        set(EXPECTED_SFML_HASH "SHA256=4D83F919BD9356CD8C9C7B542BF70E4F6E542A51FB0FD1B6DE7AFFC81ED919FB")
    endif()
    set(SFML_ZIP_PATH "${CMAKE_BINARY_DIR}/sfml-3.0.0-win.zip")
    # S6-TV1-27: TLS verification is ON and the archive integrity is verified via
    # EXPECTED_SFML_HASH (real SHA256 of the downloaded zip, confirmed by TV1).
    file(DOWNLOAD "${SFML_URL}"
                  "${SFML_ZIP_PATH}"
                  TLS_VERIFY ON
                  EXPECTED_HASH "${EXPECTED_SFML_HASH}"
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

# Find SFML 3.0 with required components or fallback to FetchContent
include(FetchContent)
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio QUIET)
if(NOT SFML_FOUND)
    message(STATUS "SFML 3.0 not found locally or on system. Fetching SFML 3.0.0 via FetchContent...")
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.0.0
    )
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SFML)
endif()

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

# Remove main.cpp and demo files from SOURCES to avoid multiple definition of main in game_lib and tests
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
list(FILTER SOURCES EXCLUDE REGEX "src/demo/.*\\.cpp$")

# Add global compiler options (must be set before targets)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    add_compile_options(-Wall -Wextra -pedantic)
elseif(MSVC)
    add_compile_options(/W4 /permissive- /FS)
endif()

# Core logic static library
add_library(game_lib STATIC ${SOURCES})
target_include_directories(game_lib PUBLIC include)
target_link_libraries(game_lib PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    box2d
)

# Executable target
add_executable(SuperMario src/main.cpp)
target_link_libraries(SuperMario PRIVATE game_lib)

# ==============================================================================
# 4. TARGET BUILD CONFIGURATIONS
# ==============================================================================
# Define DEBUG macro for debug builds
target_compile_definitions(SuperMario PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(game_lib PRIVATE $<$<CONFIG:Debug>:DEBUG>)

include(CTest)
if(BUILD_TESTING)
    add_executable(input_state_tests
        tests/InputStateTests.cpp

    )
    target_link_libraries(input_state_tests PRIVATE game_lib)
    add_test(NAME input_state_tests COMMAND input_state_tests)

    add_executable(tile_collision_span_tests
        tests/TileCollisionSpanTests.cpp
    )
    target_link_libraries(tile_collision_span_tests PRIVATE game_lib)
    add_test(NAME tile_collision_span_tests COMMAND tile_collision_span_tests)

    add_executable(mario_physics_tests
        tests/MarioPhysicsTests.cpp
    )
    target_link_libraries(mario_physics_tests PRIVATE game_lib)
    add_test(NAME mario_physics_tests COMMAND mario_physics_tests)

    add_executable(play_state_tests
        tests/PlayStateTests.cpp
    )
    target_link_libraries(play_state_tests PRIVATE game_lib)
    add_test(NAME play_state_tests COMMAND play_state_tests)
    set_tests_properties(play_state_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(game_manager_tests
        tests/GameManagerTests.cpp
        src/core/GameManager.cpp
        src/core/SaveManager.cpp
    )
    target_include_directories(game_manager_tests PRIVATE include)
    target_link_libraries(game_manager_tests PRIVATE SFML::Graphics)
    add_test(NAME game_manager_tests COMMAND game_manager_tests)

    add_executable(event_bus_tests
        tests/EventBusTests.cpp
        src/patterns/EventBus.cpp
    )
    target_include_directories(event_bus_tests PRIVATE include)
    add_test(NAME event_bus_tests COMMAND event_bus_tests)

    add_executable(tv5_integration_tests
        tests/TV5IntegrationTests.cpp
    )
    target_link_libraries(tv5_integration_tests PRIVATE game_lib)
    add_test(NAME tv5_integration_tests COMMAND tv5_integration_tests)

    # S6-TV1-05/06/07/08 — New headless test for level-catalog & GameProgress contracts.
    add_executable(level_catalog_tests
        tests/LevelCatalogTests.cpp
    )
    target_include_directories(level_catalog_tests PRIVATE include)
    target_link_libraries(level_catalog_tests PRIVATE game_lib)
    add_test(NAME level_catalog_tests COMMAND level_catalog_tests)

    add_executable(save_manager_tests
        tests/SaveManagerTests.cpp
    )
    target_link_libraries(save_manager_tests PRIVATE game_lib)
    add_test(NAME save_manager_tests COMMAND save_manager_tests)

    add_executable(level_validator_tests
        tests/LevelValidatorTests.cpp
    )
    target_link_libraries(level_validator_tests PRIVATE game_lib)
    add_test(NAME level_validator_tests COMMAND level_validator_tests)

    add_executable(springboard_tests
        tests/SpringboardTests.cpp
    )
    target_link_libraries(springboard_tests PRIVATE game_lib)
    add_test(NAME springboard_tests COMMAND springboard_tests)

    add_executable(elevator_tests
        tests/ElevatorTests.cpp
    )
    target_link_libraries(elevator_tests PRIVATE game_lib)
    add_test(NAME elevator_tests COMMAND elevator_tests)
    set_tests_properties(elevator_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(cheep_cheep_tests
        tests/CheepCheepTests.cpp
    )
    target_link_libraries(cheep_cheep_tests PRIVATE game_lib)
    add_test(NAME cheep_cheep_tests COMMAND cheep_cheep_tests)
    set_tests_properties(cheep_cheep_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    set_tests_properties(level_validator_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    # Gate 0 release-contract guards (S6-TV1 Gate 0 / test contract).
    add_executable(gate0_contract_tests
        tests/Gate0ContractTests.cpp
    )
    target_link_libraries(gate0_contract_tests PRIVATE game_lib)
    add_test(NAME gate0_contract_tests COMMAND gate0_contract_tests)
    set_tests_properties(gate0_contract_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    # SaveManager session-restart integration (S6-TV1-19).
    add_executable(save_session_tests
        tests/SaveSessionTests.cpp
    )
    target_link_libraries(save_session_tests PRIVATE game_lib)
    add_test(NAME save_session_tests COMMAND save_session_tests)
    set_tests_properties(save_session_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(sprite_frames_theme_tests
        tests/SpriteFramesThemeTests.cpp
    )
    target_link_libraries(sprite_frames_theme_tests PRIVATE game_lib)
    add_test(NAME sprite_frames_theme_tests COMMAND sprite_frames_theme_tests)

    add_executable(collision_matrix_tests
        tests/CollisionMatrixTests.cpp
    )
    target_link_libraries(collision_matrix_tests PRIVATE game_lib)
    add_test(NAME collision_matrix_tests COMMAND collision_matrix_tests)
    set_tests_properties(collision_matrix_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(fireball_request_tests
        tests/FireBallRequestTests.cpp
    )
    target_link_libraries(fireball_request_tests PRIVATE game_lib)
    add_test(NAME fireball_request_tests COMMAND fireball_request_tests)
    set_tests_properties(fireball_request_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(display_camera_ui_tests
        tests/DisplayCameraUITests.cpp
    )
    target_link_libraries(display_camera_ui_tests PRIVATE game_lib)
    add_test(NAME display_camera_ui_tests COMMAND display_camera_ui_tests)
    set_tests_properties(display_camera_ui_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(character_flow_tests
        tests/CharacterFlowTests.cpp
    )
    target_link_libraries(character_flow_tests PRIVATE game_lib)
    add_test(NAME character_flow_tests COMMAND character_flow_tests)
    set_tests_properties(character_flow_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(stomp_score_tests
        tests/StompScoreTests.cpp
    )
    target_link_libraries(stomp_score_tests PRIVATE game_lib)
    add_test(NAME stomp_score_tests COMMAND stomp_score_tests)
    set_tests_properties(stomp_score_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    # PvP duel mode (2 PLAYER VERSUS): arena validation, contact rules and
    # the round/fire-flower state machine.
    add_executable(pvp_arena_tests
        tests/PvpArenaTests.cpp
    )
    target_link_libraries(pvp_arena_tests PRIVATE game_lib)
    add_test(NAME pvp_arena_tests COMMAND pvp_arena_tests)
    set_tests_properties(pvp_arena_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(pvp_stomp_tests
        tests/PvpStompTests.cpp
    )
    target_link_libraries(pvp_stomp_tests PRIVATE game_lib)
    add_test(NAME pvp_stomp_tests COMMAND pvp_stomp_tests)
    set_tests_properties(pvp_stomp_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_executable(pvp_flow_tests
        tests/PvpFlowTests.cpp
    )
    target_link_libraries(pvp_flow_tests PRIVATE game_lib)
    add_test(NAME pvp_flow_tests COMMAND pvp_flow_tests)
    set_tests_properties(pvp_flow_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    # Keep test assertions and assertion-contained setup active in optimized
    # configurations.  CMake adds -DNDEBUG (/DNDEBUG) for Release-like builds;
    # undefine it only on the registered test executables, leaving game_lib and
    # SuperMario with their normal production NDEBUG semantics.
    set(SUPERMARIO_TEST_TARGETS
        input_state_tests
        tile_collision_span_tests
        mario_physics_tests
        play_state_tests
        game_manager_tests
        event_bus_tests
        tv5_integration_tests
        level_catalog_tests
        save_manager_tests
        level_validator_tests
        springboard_tests
        elevator_tests
        cheep_cheep_tests
        gate0_contract_tests
        save_session_tests
        sprite_frames_theme_tests
        collision_matrix_tests
        fireball_request_tests
        display_camera_ui_tests
        character_flow_tests
        stomp_score_tests
        pvp_arena_tests
        pvp_stomp_tests
        pvp_flow_tests
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        set(SUPERMARIO_TEST_UNDEFINE_NDEBUG "-UNDEBUG")
    elseif(MSVC)
        set(SUPERMARIO_TEST_UNDEFINE_NDEBUG "/UNDEBUG")
    endif()

    if(SUPERMARIO_TEST_UNDEFINE_NDEBUG)
        foreach(SUPERMARIO_TEST_TARGET IN LISTS SUPERMARIO_TEST_TARGETS)
            target_compile_options(${SUPERMARIO_TEST_TARGET} PRIVATE
                "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:${SUPERMARIO_TEST_UNDEFINE_NDEBUG}>"
            )
        endforeach()
    endif()
endif()

# Platform-specific Release build settings
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(WIN32)
        set_property(TARGET SuperMario PROPERTY WIN32_EXECUTABLE TRUE)
    elseif(APPLE)
        set_property(TARGET SuperMario PROPERTY MACOSX_BUNDLE TRUE)
    endif()
endif()

# ==============================================================================
# 5. POST-BUILD ACTIONS
# ==============================================================================

# 5.1. Package only the release runtime inputs next to the executable.
#
# Keep this list explicit and source-relative.  A recursive copy silently pulls
# fixtures, future content, and documentation/reference files into an RC.  The
# configure-time checks make a missing or accidentally renamed input fail with a
# path-specific diagnostic before a package can be built.
set(SUPERMARIO_RUNTIME_ASSETS
    "fonts/mario.ttf"
    "ui/bg_world.png"
    "textures/ui/hud.png"
    "textures/ui/stage_1.png"
    "textures/ui/stage_2.png"
    "textures/ui/stage_3.png"
    "textures/ui/stage_4.png"
    "textures/mario/MarioLuigi.png"
    "textures/tiles/tileset.png"
    "textures/items/items_blocks.png"
    "textures/items/items_objects.png"
    "textures/enemies/enemies.png"
    "sounds/effects/1up.wav"
    "sounds/effects/brick.wav"
    "sounds/effects/bump.wav"
    "sounds/effects/coin.wav"
    "sounds/effects/death.wav"
    "sounds/effects/fireball.wav"
    "sounds/effects/flagpole.wav"
    "sounds/effects/gameover.wav"
    "sounds/effects/hurryup.wav"
    "sounds/effects/item.wav"
    "sounds/effects/jump.wav"
    "sounds/effects/jumpsmall.wav"
    "sounds/effects/kickkill.wav"
    "sounds/effects/pause.wav"
    "sounds/effects/pipepowerdown.wav"
    "sounds/effects/powerup.wav"
    "sounds/effects/stompswim.wav"
    "sounds/music/overworld.flac"
    "sounds/music/underground.flac"
    "sounds/music/underwater.flac"
    "sounds/music/castle.flac"
    "sounds/music/invincible.flac"
    "sounds/music/death.flac"
    "sounds/music/gameover.flac"
    "sounds/music/level_complete.flac"
)

set(SUPERMARIO_RUNTIME_LEVELS
    "level1.txt"
    "level2.txt"
    "level3.txt"
    "level4.txt"
    # Runtime tolerates these registries when absent, but the RC includes them
    # so configured Cheep Cheep/elevator routes remain reproducible.
    "cheep_cheep.txt"
    "elevators.txt"
    # 2 PLAYER VERSUS duel arena (loaded directly by PvpPlayState).
    "pvp_arena.txt"
)

# TARGET_FILE_DIR is the package root for both single- and multi-config
# generators (e.g. the Debug subdirectory under Visual Studio/Xcode).
set(SUPERMARIO_PACKAGE_ROOT "$<TARGET_FILE_DIR:SuperMario>")

foreach(RUNTIME_ASSET_REL IN LISTS SUPERMARIO_RUNTIME_ASSETS)
    set(RUNTIME_ASSET_SOURCE "${CMAKE_SOURCE_DIR}/assets/${RUNTIME_ASSET_REL}")
    if(NOT EXISTS "${RUNTIME_ASSET_SOURCE}" OR IS_DIRECTORY "${RUNTIME_ASSET_SOURCE}")
        message(FATAL_ERROR
            "Required runtime asset is missing or is not a file: ${RUNTIME_ASSET_SOURCE}"
        )
    endif()
    get_filename_component(RUNTIME_ASSET_DIR "${RUNTIME_ASSET_REL}" DIRECTORY)
    add_custom_command(TARGET SuperMario POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${SUPERMARIO_PACKAGE_ROOT}/assets/${RUNTIME_ASSET_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${RUNTIME_ASSET_SOURCE}"
            "${SUPERMARIO_PACKAGE_ROOT}/assets/${RUNTIME_ASSET_REL}"
        COMMENT "Packaging runtime asset ${RUNTIME_ASSET_REL}"
    )
endforeach()

foreach(RUNTIME_LEVEL_REL IN LISTS SUPERMARIO_RUNTIME_LEVELS)
    set(RUNTIME_LEVEL_SOURCE "${CMAKE_SOURCE_DIR}/levels/${RUNTIME_LEVEL_REL}")
    if(NOT EXISTS "${RUNTIME_LEVEL_SOURCE}" OR IS_DIRECTORY "${RUNTIME_LEVEL_SOURCE}")
        message(FATAL_ERROR
            "Required runtime level/config is missing or is not a file: ${RUNTIME_LEVEL_SOURCE}"
        )
    endif()
    get_filename_component(RUNTIME_LEVEL_DIR "${RUNTIME_LEVEL_REL}" DIRECTORY)
    add_custom_command(TARGET SuperMario POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${SUPERMARIO_PACKAGE_ROOT}/levels/${RUNTIME_LEVEL_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${RUNTIME_LEVEL_SOURCE}"
            "${SUPERMARIO_PACKAGE_ROOT}/levels/${RUNTIME_LEVEL_REL}"
        COMMENT "Packaging runtime level/config ${RUNTIME_LEVEL_REL}"
    )
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
            add_custom_command(TARGET SuperMario POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL_PATH}" "$<TARGET_FILE_DIR:SuperMario>/"
                COMMENT "Copying ${DLL_NAME} to build output folder..."
            )
        endif()
    endforeach()
endif()

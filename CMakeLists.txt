cmake_minimum_required(VERSION 3.16)
project(SuperMario VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# Ưu tiên tìm SFML 3.0 trong thirdparty/SFML nếu tồn tại
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
endif()

# Tìm SFML 3.0 (MinGW/MSYS2 hoặc từ thirdparty)
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)

# Nạp tất cả source code
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# Cấu hình compiler (MinGW/GCC)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

# Ẩn console khi build Release trên Windows
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# Thư mục include
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Liên kết SFML 3 (Target names thay đổi thành SFML::Component)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# Tự động copy assets & levels sau khi build
foreach(DIR "assets" "levels")
    if(EXISTS "${CMAKE_SOURCE_DIR}/${DIR}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DIR}"
            COMMENT "Copying ${DIR} to output directory..."
        )
    endif()
endforeach()

# Tự động copy SFML 3 DLLs (để chạy file .exe độc lập)
if(WIN32)
    # Xác định thư mục chứa DLL
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML/bin")
        set(SFML_DLL_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SFML/bin")
    else()
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(SFML_DLL_DIR "${MINGW_BIN_DIR}")
    endif()

    set(SFML_MINGW_DLLS
        "${SFML_DLL_DIR}/sfml-graphics-3.dll"
        "${SFML_DLL_DIR}/sfml-graphics-d-3.dll"
        "${SFML_DLL_DIR}/sfml-window-3.dll"
        "${SFML_DLL_DIR}/sfml-window-d-3.dll"
        "${SFML_DLL_DIR}/sfml-system-3.dll"
        "${SFML_DLL_DIR}/sfml-system-d-3.dll"
        "${SFML_DLL_DIR}/sfml-audio-3.dll"
        "${SFML_DLL_DIR}/sfml-audio-d-3.dll"
    )
    foreach(DLL_FILE IN LISTS SFML_MINGW_DLLS)
        if(EXISTS "${DLL_FILE}")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL_FILE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
                COMMENT "Copying ${DLL_FILE}..."
            )
        endif()
    endforeach()
endif()

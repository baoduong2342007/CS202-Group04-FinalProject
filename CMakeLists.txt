cmake_minimum_required(VERSION 3.16)
project(SuperMario VERSION 1.0 LANGUAGES CXX)

# Bắt buộc dùng C++17 chuẩn, vô hiệu hóa các extension riêng của compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==============================================================================
# Cấu hình SFML từ thư mục thirdparty
# ==============================================================================
set(SFML_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SFML/lib/cmake/SFML" CACHE PATH "Path to SFML CMake config")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/thirdparty/SFML")

# Yêu cầu SFML 2.6.2 và các module cần thiết
find_package(SFML 2.6.2 COMPONENTS graphics window system audio REQUIRED)

# ==============================================================================
# Thu thập mã nguồn và tạo Executable
# ==============================================================================
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# ==============================================================================
# Cấu hình Build nâng cao (TỐI ƯU THÊM)
# ==============================================================================

# 1. Bật cảnh báo (Warnings) để giúp nhóm code sạch hơn, phát hiện lỗi sớm
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# 2. Ẩn Console trên Windows: CHỈ ẨN KHI BUILD RELEASE
# Lúc Debug vẫn giữ lại Console để xem std::cout và log lỗi
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# ==============================================================================
# Liên kết Header và Thư viện
# ==============================================================================
# Khai báo đường dẫn include code của dự án
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Liên kết SFML (Dùng PRIVATE theo chuẩn Modern CMake)
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# ==============================================================================
# Xử lý Copy Tài nguyên tự động sau khi Build (POST_BUILD)
# ==============================================================================

# 1. Copy thư mục assets và levels
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/levels" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/levels"
    COMMENT "Copying assets and levels to output directory..."
)

# 2. Copy TỰ ĐỘNG các file DLL của SFML vào thư mục Build (Cực kỳ quan trọng trên Windows)
# Việc này giúp game chạy được ngay mà không cần copy tay file .dll
if(WIN32)
    file(GLOB SFML_DLLS "${CMAKE_SOURCE_DIR}/thirdparty/SFML/bin/*.dll")
    if(SFML_DLLS)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_DLLS} "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
            COMMENT "Copying SFML DLLs to output directory..."
        )
    endif()
endif()

cmake_minimum_required(VERSION 3.10)
project(CZDBSearcher C)

set(CMAKE_C_STANDARD 99)

# Add the dynamic library
add_library(db_searcher SHARED db_searcher.c byte_utils.c hyper_header_block.c decrypted_block.c)

add_executable(CZDBSearcher main.c hyper_header_block.c decrypted_block.c
        byte_utils.c
        byte_utils.h)

# Link the dynamic library to your executable
target_link_libraries(CZDBSearcher db_searcher)

# 如果您的项目需要OpenSSL，您需要找到它并链接到您的项目
find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    target_link_libraries(db_searcher ${OPENSSL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenSSL not found")
endif()

# set the compiler flags
set(CMAKE_C_FLAGS "-g")
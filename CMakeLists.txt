cmake_minimum_required(VERSION 3.10)
project(CZDBSearcher C)

set(CMAKE_C_STANDARD 99)

# Platform detection
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
endif()

# Source files for library
set(LIB_SOURCES
    db_searcher.c 
    byte_utils.c 
    hyper_header_block.c 
    decrypted_block.c
)

# Source files for executable
set(EXE_SOURCES
    main.c 
    hyper_header_block.c 
    decrypted_block.c
    byte_utils.c
    byte_utils.h
)

# Add getopt for Windows
if(WIN32)
    list(APPEND EXE_SOURCES getopt.c)
endif()

# Add the dynamic library
add_library(db_searcher SHARED ${LIB_SOURCES})

# Define macro for DLL export when building the library
target_compile_definitions(db_searcher PRIVATE BUILDING_DB_SEARCHER_DLL)

# Set library output name
if(WIN32)
    set_target_properties(db_searcher PROPERTIES OUTPUT_NAME "db_searcher")
    set_target_properties(db_searcher PROPERTIES PREFIX "")
    set_target_properties(db_searcher PROPERTIES SUFFIX ".dll")
endif()

add_executable(CZDBSearcher ${EXE_SOURCES})

# Link the dynamic library to your executable
target_link_libraries(CZDBSearcher db_searcher)

# Platform-specific network libraries
if(WIN32)
    target_link_libraries(db_searcher ws2_32)
    target_link_libraries(CZDBSearcher ws2_32)
endif()

# Find and link OpenSSL
find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    target_link_libraries(db_searcher ${OPENSSL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenSSL not found")
endif()

# Handle MsgPack differently for Windows and Unix
if(WIN32)
    # For Windows, try to find msgpack using pkg-config or manual search
    find_path(MSGPACK_INCLUDE_DIR msgpack.h
        PATHS 
        ${CMAKE_PREFIX_PATH}/include
        $ENV{MSGPACK_ROOT}/include
        C:/vcpkg/installed/x64-windows/include
        NO_DEFAULT_PATH
    )
    
    find_library(MSGPACK_LIBRARY 
        NAMES msgpack-c msgpack
        PATHS 
        ${CMAKE_PREFIX_PATH}/lib
        $ENV{MSGPACK_ROOT}/lib
        C:/vcpkg/installed/x64-windows/lib
        NO_DEFAULT_PATH
    )
    
    if(MSGPACK_INCLUDE_DIR AND MSGPACK_LIBRARY)
        include_directories(${MSGPACK_INCLUDE_DIR})
        target_link_libraries(db_searcher ${MSGPACK_LIBRARY})
        target_link_libraries(CZDBSearcher ${MSGPACK_LIBRARY})
    else()
        message(FATAL_ERROR "MsgPack not found. Please install msgpack-c or set MSGPACK_ROOT")
    endif()
else()
    # Set the default value for MSGPACK_ROOT for Unix systems
    set(MSGPACK_ROOT "/usr/local" CACHE PATH "Default path to MsgPack")

    # Include msgpack headers
    include_directories(${MSGPACK_ROOT}/include)

    # Link msgpack library
    find_library(MSGPACK_LIBRARY msgpack-c ${MSGPACK_ROOT}/lib)
    if(MSGPACK_LIBRARY)
        target_link_libraries(db_searcher ${MSGPACK_LIBRARY})
        target_link_libraries(CZDBSearcher ${MSGPACK_LIBRARY})
    else()
        message(FATAL_ERROR "MsgPack not found")
    endif()
endif()

# Create a config header for platform detection
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
cmake_minimum_required(VERSION 3.20)
project(m3ds)

set(PLATFORM 3DS)

string(REPLACE "/opt/" "C:/" DEVKITPRO "$ENV{DEVKITPRO}")
string(REPLACE "/opt/" "C:/" DEVKITARM "$ENV{DEVKITARM}")

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/output")
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

if (${PLATFORM} STREQUAL 3DS)
    set(CMAKE_C_COMPILER "${DEVKITPRO}/devkitARM/bin/arm-none-eabi-gcc.exe")
    set(CMAKE_CXX_COMPILER "${DEVKITPRO}/devkitARM/bin/arm-none-eabi-g++.exe")
    set(CMAKE_AR "${DEVKITPRO}/devkitARM/bin/arm-none-eabi-ar.exe")
    set(CMAKE_OBJCOPY "${DEVKITPRO}/devkitARM/bin/arm-none-eabi-objcopy.exe")
    set(CMAKE_SIZE "${DEVKITPRO}/devkitARM/bin/arm-none-eabi-size.exe")

    set(CMAKE_EXE_LINKER_FLAGS_INIT "")

    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR armv6k)

    include_directories(SYSTEM ${DEVKITARM}/include/ ${DEVKITPRO}/libctru/include/ ${DEVKITPRO}/devkitARM/arm-none-eabi/include/)
    include_directories(${BUILD_DIR} ${INCLUDE_DIR})

    set(ARCH "-march=armv6k -mtune=mpcore -mfloat-abi=hard")
    set(COMMON_FLAGS "${ARCH} -mword-relocations -fomit-frame-pointer -D__3DS__")
elseif(${PLATFORM} STREQUAL Windows)
    set(COMMON_FLAGS "-DM3DS_SFML")
endif()

set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -Wextra -Wconversion -Wno-psabi -fno-rtti -fno-exceptions")

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 26)

file(GLOB_RECURSE HEADER_FILES
        "${INCLUDE_DIR}/*.h"
        "${INCLUDE_DIR}/*.hpp"
)

file(GLOB_RECURSE SOURCE_FILES
        "${SOURCE_DIR}/*.c"
        "${SOURCE_DIR}/*.cpp"
)

set(ALL_FILES ${HEADER_FILES} ${SOURCE_FILES})

add_library(m3ds STATIC ${ALL_FILES})

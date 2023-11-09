cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)

project(freertos-mem)

file(GLOB FREERTOS_SRC_FILES ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/*.c)

add_executable(main
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${FREERTOS_SRC_FILES}
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_3.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/port.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
    ${FREERTOS_TRACE_SRC_FILES}
)

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils
)

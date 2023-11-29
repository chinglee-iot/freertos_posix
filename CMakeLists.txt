cmake_minimum_required(VERSION 3.18)

enable_testing()

set(CMAKE_CXX_STANDARD 17)

project(freertos-mem)

# Set a default build type if none was specified
set(default_build_type "Debug")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(VALGRIND valgrind)


# helper function that adds a test + a valgrind variant
function(register_test test_name executable_name)
    add_test(NAME ${test_name} COMMAND ${executable_name})
    if (VALGRIND_FOUND)
        add_test(NAME ${test_name}_valgrind COMMAND valgrind --leak-check=full --error-exitcode=100 ${executable_name})
    endif()
endfunction()

file(GLOB FREERTOS_SRC_FILES ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/*.c)

function(make_freertos_executable name files freertos defines)
add_executable(${name}
    ${files}
    ${CMAKE_SOURCE_DIR}/freertos_functions.cpp
    ${FREERTOS_SRC_FILES}
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_3.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/port.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
    ${FREERTOS_TRACE_SRC_FILES}
)

target_include_directories(${name} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils
)

target_compile_definitions(${name} PUBLIC ${defines})

register_test(${name} $<TARGET_FILE:${name}>)

endfunction()


set(FREERTOS_UPSTREAM ${CMAKE_SOURCE_DIR}/FreeRTOS_upstream)
set(FREERTOS_NEW ${CMAKE_SOURCE_DIR}/FreeRTOS)

make_freertos_executable(simple_task_end simple_task_end.cpp ${FREERTOS_NEW} "configUSE_IDLE_HOOK=0")
make_freertos_executable(simple_task_end_2 simple_task_end_2.cpp ${FREERTOS_NEW} "configUSE_IDLE_HOOK=0")

make_freertos_executable(simple_static_task_end simple_static_task_end.cpp ${FREERTOS_NEW} "configUSE_IDLE_HOOK=0")
make_freertos_executable(simple_static_task_end_2 simple_static_task_end_2.cpp ${FREERTOS_NEW} "configUSE_IDLE_HOOK=0")

make_freertos_executable(signal_test signal_test.cpp ${FREERTOS_NEW} "configUSE_IDLE_HOOK=0")

make_freertos_executable(restart_scheduler restart_scheduler.cpp ${FREERTOS_NEW} "configUSE_IDLE_HOOK=0")

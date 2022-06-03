cmake_minimum_required(VERSION 3.14.5)

execute_process(
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/VERSION
    OUTPUT_VARIABLE Version
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

project(prometheus-client-c VERSION ${Version} LANGUAGES C)

add_subdirectory (prom)

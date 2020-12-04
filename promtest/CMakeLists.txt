cmake_minimum_required(VERSION 3.14.5)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

execute_process(
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../VERSION
    OUTPUT_VARIABLE Version
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../VERSION
    COMMAND sed --regexp-extended "s/([0-9]+)\.([0-9])+\.([0-9]+)/\\1/g"
    OUTPUT_VARIABLE MajorVersion
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../VERSION
    COMMAND sed --regexp-extended "s/([0-9]+)\.([0-9])+\.([0-9]+)/\\2/"
    OUTPUT_VARIABLE MinorVersion
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../VERSION
    COMMAND sed --regexp-extended "s/([0-9]+)\.([0-9])+\.([0-9]+)/\\3/"
    OUTPUT_VARIABLE PatchVersion
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

project(promtest VERSION ${Version} LANGUAGES C)

set(prom_deb ${CMAKE_CURRENT_SOURCE_DIR}/../prom/libprom-dev-${Version}-Linux.deb)
set(promhttp_deb ${CMAKE_CURRENT_SOURCE_DIR}/../promhttp/libpromhttp-dev-${Version}-Linux.deb)

execute_process(
    COMMAND dpkg -i ${prom_deb}
    RESULT_VARIABLE prom_install_result
)

if(${prom_install_result} GREATER 0)
    message(FATAL_ERROR "failed to install ${prom_deb}")
endif()

execute_process(
    COMMAND dpkg -i ${promhttp_deb}
    RESULT_VARIABLE promhttp_install_result
)

if(${promhttp_install_result} GREATER 0)
    message(FATAL_ERROR "failed to install ${promhttp_deb}")
endif()

enable_testing()
set(vendor_dir ${CMAKE_SOURCE_DIR}/../vendor)
set(test_dir ${CMAKE_SOURCE_DIR}/test)

# Unity Dependency
set(unity_version 2.4.3)
set(unity_output ${build_dir}/unity-v${unity_version}.tar.gz)
set(unity_package ${vendor_dir}/unity)
set(unity_src ${vendor_dir}/Unity-${unity_version}/src)

if(NOT EXISTS ${vendor_dir}/Unity-${unity_version})
    execute_process(
        COMMAND curl -L https://github.com/ThrowTheSwitch/Unity/archive/v${unity_version}.tar.gz
        COMMAND tar xzvf - -C ${vendor_dir}
    )
endif()

set(
    unity_files
    ${unity_src}/unity.c
    ${unity_src}/unity.h
    ${unity_src}/unity_internals.h
)

add_library(Unity SHARED)
target_include_directories(Unity PUBLIC ${unity_src})
target_sources(Unity PUBLIC ${unity_files})
target_compile_options(Unity PUBLIC "-D UNITY_INCLUDE_DOUBLE")


# Parson dependency
set(parson_src ${vendor_dir}/parson)

if (NOT EXISTS ${vendor_dir}/parson)
    execute_process(
        COMMAND git clone https://github.com/kgabis/parson ${vendor_dir}/parson
    )
endif()

set(
    parson_files
    ${parson_src}/parson.c
    ${parson_src}/parson.h
)

add_library(parson SHARED)
target_include_directories(parson PUBLIC ${parson_src})
target_sources(parson PUBLIC ${parson_files})

include(FindThreads)

add_executable(
    promtest
    ${test_dir}/promtest.c
    ${test_dir}/promtest_helpers.c
    ${test_dir}/promtest_helpers.h
    ${test_dir}/promtest_counter.c
    ${test_dir}/promtest_counter.h
    ${test_dir}/promtest_gauge.c
    ${test_dir}/promtest_gauge.h
    ${test_dir}/promtest_histogram.c
    ${test_dir}/promtest_histogram.h
)
target_link_libraries(promtest microhttpd prom promhttp Unity parson Threads::Threads)
target_include_directories(promtest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(
    NAME promtest
    COMMAND promtest
)

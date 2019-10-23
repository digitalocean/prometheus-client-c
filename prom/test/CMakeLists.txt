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

# promTest library exposes the headers in src for testing
add_library(promTest STATIC)
target_compile_options(promTest PUBLIC "-g3" "-Wall" "-DPROM_PROCESS_LIMITS_TEST_FILE_PATH=1" "-DPROM_ASSERT_ENABLE" "-DPROM_LOG_ENABLE")
target_include_directories(
    promTest
    PUBLIC ${public_dir} ${private_dir} ${test_dir}
)
target_sources(promTest PUBLIC ${public_files} ${private_files})

include(FindThreads)

function(register_test test_name)
    add_executable(${test_name} ${test_dir}/${test_name}.c ${test_dir}/prom_test_helpers.h ${test_dir}/prom_test_helpers.c)
    target_link_libraries(${test_name} Unity promTest Threads::Threads)
    add_test(
        NAME ${test_name}
        COMMAND ${test_name}
    )
endfunction()

foreach(
    t
    prom_gauge_test
    prom_collector_test
    prom_collector_registry_test
    prom_counter_test
    prom_linked_list_test
    prom_histogram_test
    prom_histogram_buckets_test
    prom_map_test
    prom_metric_formatter_test
    prom_metric_test
    prom_metric_sample_test
    prom_process_limits_test
    prom_string_builder_test
    prom_procfs_test

)
    register_test(${t})
endforeach()

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

project(libprom VERSION ${Version} LANGUAGES C)

set(build_dir ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(public_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(private_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(test_dir ${CMAKE_CURRENT_SOURCE_DIR}/test)

set(
    public_files
    ${public_dir}/prom_alloc.h
    ${public_dir}/prom_collector.h
    ${public_dir}/prom_collector_registry.h
    ${public_dir}/prom_counter.h
    ${public_dir}/prom_gauge.h
    ${public_dir}/prom_histogram.h
    ${public_dir}/prom_histogram_buckets.h
    ${public_dir}/prom_linked_list.h
    ${public_dir}/prom_map.h
    ${public_dir}/prom_metric.h
    ${public_dir}/prom_metric_sample.h
    ${public_dir}/prom_metric_sample_histogram.h
    ${public_dir}/prom.h
)

set(
    private_files
    ${private_dir}/prom_assert.h
    ${private_dir}/prom_collector.c
    ${private_dir}/prom_collector_registry.c
    ${private_dir}/prom_collector_registry_i.h
    ${private_dir}/prom_collector_registry_t.h
    ${private_dir}/prom_collector_t.h
    ${private_dir}/prom_counter.c
    ${private_dir}/prom_gauge.c
    ${private_dir}/prom_histogram.c
    ${private_dir}/prom_histogram_buckets.c
    ${private_dir}/prom_linked_list.c
    ${private_dir}/prom_linked_list_i.h
    ${private_dir}/prom_linked_list_t.h
    ${private_dir}/prom_log.h
    ${private_dir}/prom_map.c
    ${private_dir}/prom_map_i.h
    ${private_dir}/prom_map_t.h
    ${private_dir}/prom_metric.c
    ${private_dir}/prom_metric_formatter.c
    ${private_dir}/prom_metric_formatter_i.h
    ${private_dir}/prom_metric_formatter_t.h
    ${private_dir}/prom_metric_i.h
    ${private_dir}/prom_metric_sample.c
    ${private_dir}/prom_metric_sample_histogram.c
    ${private_dir}/prom_metric_sample_histogram_i.h
    ${private_dir}/prom_metric_sample_histogram_t.h
    ${private_dir}/prom_metric_sample_i.h
    ${private_dir}/prom_metric_sample_t.h
    ${private_dir}/prom_metric_t.h
    ${private_dir}/prom_process_fds.c
    ${private_dir}/prom_process_fds_i.h
    ${private_dir}/prom_process_fds_t.h
    ${private_dir}/prom_process_limits.c
    ${private_dir}/prom_process_limits_i.h
    ${private_dir}/prom_process_limits_t.h
    ${private_dir}/prom_process_stat.c
    ${private_dir}/prom_process_stat_i.h
    ${private_dir}/prom_process_stat_t.h
    ${private_dir}/prom_procfs_i.h
    ${private_dir}/prom_procfs_t.h
    ${private_dir}/prom_procfs.c
    ${private_dir}/prom_string_builder.c
    ${private_dir}/prom_string_builder_i.h
    ${private_dir}/prom_string_builder_t.h
)

include(FindThreads)

add_library(prom SHARED)

target_compile_options(prom PRIVATE "-Werror" "-Wuninitialized" "-Wall" "-Wno-unused-label" "-std=gnu11")
target_compile_options(prom PUBLIC "-Werror" "-Wuninitialized" "-Wall" "-Wno-unused-label" "-std=gnu11")

target_include_directories(
    prom
    PUBLIC ${public_dir}
    PRIVATE ${private_dir}
)

target_sources(
    prom
    PUBLIC ${public_files}
    PRIVATE ${private_files}
)

target_link_libraries(prom PUBLIC Threads::Threads)

if ($ENV{TEST})
    include(test/CMakeLists.txt)
endif()

set(CPACK_PACKAGE_NAME libprom-dev)
set(CPACK_GENERATOR TGZ;DEB)
set(CPACK_PACKAGE_VENDOR DigitalOcean)
set(CPACK_PACKAGE_VERSION_MAJOR ${MajorVersion})
set(CPACK_PACKAGE_VERSION_MINOR ${MinorVersion})
set(CPACK_PACKAGE_VERSION_PATCH ${PatchVersion})
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../README.md)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A prometheus client library")
set(CPACK_PACKAGE_HOMEPAGE_URL https://github.internal.digitalocean.com/timeseries/prometheus-client-c)

include(CPack)
include(GNUInstallDirs)
install(TARGETS prom ARCHIVE)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

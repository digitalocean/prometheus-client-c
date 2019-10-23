/*
Copyright 2019 DigitalOcean Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef PROM_COLLECTOR_H
#define PROM_COLLECTOR_H

#include "prom_map.h"
#include "prom_metric.h"

/**
 * @file prom_collector.h
 * @brief A Prometheus collector returns a collection of metrics
 */

/**
 * @brief A prometheus collector calls collect to prepare metrics and return them to the registry to which it is
 * registered.
 */
typedef struct prom_collector prom_collector_t;

/**
 * @brief The function responsible for preparing metric data and returning metrics for a given collector.
 *
 * If you use the default collector registry, this should not concern you. If you are using a custom collector, you may
 * which to set this function on your collector to do additional work before returning the contained metrics.
 */
typedef prom_map_t* prom_collect_fn(prom_collector_t* self);

/**
 * @brief Create a collector
 */
prom_collector_t* prom_collector_new(const char *name);

/**
 * @brief Return a collector including the default process metrics
 * @param limits_path Pass NULL to discover the path to the proc/[pid]/limits file associated with process ID assigned
 * by the host environment. Otherwise, pass a string to said path.
 * @param stat_path Pass NULL to discover the path to the proc/[pid]/stat file associated with process ID assigned
 * by the host environment. Otherwise, pass a string to said path.
 */
prom_collector_t* prom_collector_process_new(const char *limits_path, const char *stat_path);

/**
 * @brief Destroy a collector
 */
int prom_collector_destroy(prom_collector_t *self);

void prom_collector_free_generic(void *gen);

/**
 * @brief Destroy a collector
 */
int prom_collector_destroy_generic(void *gen);

/**
 * @brief Add a metric to a collector
 */
int prom_collector_add_metric(prom_collector_t *self, prom_metric_t *metric);

/**
 * @brief The collect function is responsible for doing any work involving a set of metrics and then returning them
 * for metric exposition
 */
int prom_collector_set_collect_fn(prom_collector_t *self, prom_collect_fn *fn);

#endif // PROM_COLLECTOR_H
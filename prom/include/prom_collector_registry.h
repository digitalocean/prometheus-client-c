/*
Copyright 2019-2020 DigitalOcean Inc.

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

/**
 * @file prom_collector_registry.h
 * @brief The collector registry registers collectors for metric exposition.
 */

#ifndef PROM_REGISTRY_H
#define PROM_REGISTRY_H

#include "prom_collector.h"
#include "prom_metric.h"

/**
 * @brief A prom_registry_t is responsible for registering metrics and briding them to the string exposition format
 */
typedef struct prom_collector_registry prom_collector_registry_t;

/**
 * @brief Initialize the default registry by calling prom_collector_registry_init within your program. You MUST NOT
 * modify this value.
 */
extern prom_collector_registry_t *PROM_COLLECTOR_REGISTRY_DEFAULT;

/**
 * @brief Initializes the default collector registry and enables metric collection on the executing process
 * @return A non-zero integer value upon failure
 */
int prom_collector_registry_default_init(void);

/**
 * @brief Constructs a prom_collector_registry_t*
 * @param name The name of the collector registry. It MUST NOT be default.
 * @return The constructed prom_collector_registry_t*
 */
prom_collector_registry_t *prom_collector_registry_new(const char *name);

/**
 * @brief Destroy a collector registry. You MUST set self to NULL after destruction.
 * @param self The target prom_collector_registry_t*
 * @return A non-zero integer value upon failure
 */
int prom_collector_registry_destroy(prom_collector_registry_t *self);

/**
 * @brief Enable process metrics on the given collector registry
 * @param self The target prom_collector_registry_t*
 * @return A non-zero integer value upon failure
 */
int prom_collector_registry_enable_process_metrics(prom_collector_registry_t *self);

/**
 * @brief Registers a metric with the default collector on PROM_DEFAULT_COLLECTOR_REGISTRY
 *
 * The metric to be registered MUST NOT already be registered with the given . If so, the program will
 * halt. It returns a prom_metric_t* to simplify metric creation and registration. Furthermore,
 * PROM_DEFAULT_COLLECTOR_REGISTRY must be registered via prom_collector_registry_default_init() prior to calling this
 * function. The metric will be added to the default registry's default collector.
 *
 * @param metric The metric to register on PROM_DEFAULT_COLLECTOR_REGISTRY*
 * @return The registered prom_metric_t*
 */
prom_metric_t *prom_collector_registry_must_register_metric(prom_metric_t *metric);

/**
 * @brief Registers a metric with the default collector on PROM_DEFAULT_COLLECTOR_REGISTRY. Returns an non-zero integer
 * value on failure.
 *
 * See prom_collector_registry_must_register_metric.
 *
 * @param metric The metric to register on PROM_DEFAULT_COLLECTOR_REGISTRY*
 * @return A non-zero integer value upon failure
 */
int prom_collector_registry_register_metric(prom_metric_t *metric);

/**
 * @brief Register a collector with the given registry. Returns a non-zero integer value on failure.
 * @param self The target prom_collector_registry_t*
 * @param collector The prom_collector_t* to register onto the prom_collector_registry_t* as self
 * @return A non-zero integer value upon failure
 */
int prom_collector_registry_register_collector(prom_collector_registry_t *self, prom_collector_t *collector);

/**
 * @brief Returns a string in the default metric exposition format. The string MUST be freed to avoid unnecessary heap
 * memory growth.
 *
 * Reference: https://prometheus.io/docs/instrumenting/exposition_formats/
 *
 * @param self The target prom_collector_registry_t*
 * @return The string int he default metric exposition format.
 */
const char *prom_collector_registry_bridge(prom_collector_registry_t *self);

/**
 *@brief Validates that the given metric name complies with the specification:
 *
 * Reference: https://prometheus.io/docs/concepts/data_model/#metric-names-and-labels
 *
 * Returns a non-zero integer value on failure.
 *
 * @param self The target prom_collector_registry_t*
 * @param metric_name The metric name to validate
 * @return A non-zero integer value upon failure
 */
int prom_collector_registry_validate_metric_name(prom_collector_registry_t *self, const char *metric_name);

#endif  // PROM_H

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

/**
 * @file prom_histogram.h
 * @brief https://prometheus.io/docs/concepts/metric_types/#histogram
 */

#ifndef PROM_HISTOGRAM_INCLUDED
#define PROM_HISTOGRAM_INCLUDED

#include <stdlib.h>

#include "prom_metric.h"
#include "prom_histogram_buckets.h"

/**
 * @brief A prometheus histogram.
 *
 * References
 * * See https://prometheus.io/docs/concepts/metric_types/#histogram
 */
typedef prom_metric_t prom_histogram_t;

/**
 * @brief Returns a pointer to a prom_histogram_t
 */
prom_histogram_t* prom_histogram_new(const char *name,
                                     const char *help,
                                     prom_histogram_buckets_t *buckets,
                                     size_t label_key_count,
                                     const char **label_keys);

/**
 * @brief Destroys a pointer to a prom_gauge
 */
int prom_histogram_destroy(prom_histogram_t *self);


/**
 * @brief Observe the prom_histogram_t given the value and labells
 */
int prom_histogram_observe(prom_histogram_t *self, double value, const char **label_values);

#endif  // PROM_HISTOGRAM_INCLUDED

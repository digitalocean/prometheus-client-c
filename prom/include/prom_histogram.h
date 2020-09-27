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
 * @file prom_histogram.h
 * @brief https://prometheus.io/docs/concepts/metric_types/#histogram
 */

#ifndef PROM_HISTOGRAM_INCLUDED
#define PROM_HISTOGRAM_INCLUDED

#include <stdlib.h>

#include "prom_histogram_buckets.h"
#include "prom_metric.h"

/**
 * @brief A prometheus histogram.
 *
 * References
 * * See https://prometheus.io/docs/concepts/metric_types/#histogram
 */
typedef prom_metric_t prom_histogram_t;

/**
 *@brief Construct a prom_histogram_t*
 * @param name The name of the metric
 * @param help The metric description
 * @param buckets The prom_histogram_buckets_t*. See prom_histogram_buckets.h.
 * @param label_key_count is the number of labels associated with the given metric. Pass 0 if the metric does not
 *                        require labels.
 * @param label_keys A collection of label keys. The number of keys MUST match the value passed as label_key_count. If
 *                   no labels are required, pass NULL. Otherwise, it may be convenient to pass this value as a
 *                   literal.
 * @return The constructed prom_histogram_t*
 *
 * *Example*
 *
 *     // An example with labels
 *     prom_histogram_buckets_t* buckets = prom_histogram_buckets_linear(5.0, 5.0, 10);
 *     prom_histogram_new("foo", "foo is a counter with labels", buckets, 2, (const char**) { "one", "two" });
 *
 *     // An example without labels
 *     prom_histogram_buckets_t* buckets = prom_histogram_buckets_linear(5.0, 5.0, 10);
 *     prom_histogram_new("foo", "foo is a counter without labels", buckets, 0, NULL);
 */
prom_histogram_t *prom_histogram_new(const char *name, const char *help, prom_histogram_buckets_t *buckets,
                                     size_t label_key_count, const char **label_keys);

/**
 * @brief Destroy a prom_histogram_t*. self MUSTS be set to NULL after destruction. Returns a non-zero integer value
 *        upon failure.
 * @return Non-zero value upon failure.
 */
int prom_histogram_destroy(prom_histogram_t *self);

/**
 * @brief Observe the prom_histogram_t given the value and labells
 * @param self The target prom_histogram_t*
 * @param value The value to observe
 * @param label_keys A collection of label keys. The number of keys MUST match the value passed as label_key_count. If
 *                   no labels are required, pass NULL. Otherwise, it may be convenient to pass this value as a
 *                   literal.
 * @return Non-zero value upon failure
 */
int prom_histogram_observe(prom_histogram_t *self, double value, const char **label_values);

#endif  // PROM_HISTOGRAM_INCLUDED

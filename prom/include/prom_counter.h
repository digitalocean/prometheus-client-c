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

#ifndef PROM_COUNTER_H
#define PROM_COUNTER_H

#include <stdlib.h>

#include "prom_metric.h"

/**
 * @file prom_counter.h
 * @brief https://prometheus.io/docs/concepts/metric_types/#counter
 */

/**
 * @brief A prometheus counter.
 *
 * References
 * * See https://prometheus.io/docs/concepts/metric_types/#counter
 */
typedef prom_metric_t prom_counter_t;

/**
 * @brief Construct a prom_counter_t*
 * @param name The name of the metric
 * @param help The metric description
 * @param label_key_count The number of labels associated with the given metric. Pass 0 if the metric does not
 *                        require labels.
 * @param label_keys A collection of label keys. The number of keys MUST match the value passed as label_key_count. If
 *                   no labels are required, pass NULL. Otherwise, it may be convenient to pass this value as a
 *                   literal.
 * @return The constructed prom_counter_t*
 *
 * *Example*
 *
 *     // An example with labels
 *     prom_counter_new("foo", "foo is a counter with labels", 2, (const char**) { "one", "two" });
 *
 *     // An example without labels
 *     prom_counter_new("foo", "foo is a counter without labels", 0, NULL);
 */
prom_counter_t *prom_counter_new(const char *name, const char *help, size_t label_key_count, const char **label_keys);

/**
 * @brief Destroys a prom_counter_t*. You must set self to NULL after destruction. A non-zero integer value will be
 *        returned on failure.
 * @param self A prom_counter_t*
 * @return A non-zero integer value upon failure.
 */
int prom_counter_destroy(prom_counter_t *self);

/**
 * @brief Increment the prom_counter_t by 1. A non-zero integer value will be returned on failure.
 * @param self The target  prom_counter_t*
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the counter's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return A non-zero integer value upon failure.
 *
 * *Example*
 *
 *     // An example with labels
 *     prom_counter_inc(foo_counter, (const char**) { "bar", "bang" });
 **
 *     // An example without labels
 *     prom_counter_inc(foo_counter, NULL);
 */
int prom_counter_inc(prom_counter_t *self, const char **label_values);

/**
 * @brief Add the value to the prom_counter_t*. A non-zero integer value will be returned on failure.
 * @param self The target  prom_counter_t*
 * @param r_value The double to add to the prom_counter_t passed as self. The value MUST be greater than or equal to 0.
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the counter's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return A non-zero integer value upon failure.
 *
 * *Example*
 *
 *     // An example with labels
 *     prom_counter_add(foo_counter, 22, (const char**) { "bar", "bang" });
 *
 *     // An example without labels
 *     prom_counter_add(foo_counter, 22, NULL);
 */
int prom_counter_add(prom_counter_t *self, double r_value, const char **label_values);

#endif  // PROM_COUNTER_H

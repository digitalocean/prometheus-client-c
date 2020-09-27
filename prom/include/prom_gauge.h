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
 * @file prom_gauge.h
 * @brief https://prometheus.io/docs/concepts/metric_types/#gauge
 */

#ifndef PROM_GAUGE_H
#define PROM_GAUGE_H

#include <stdlib.h>

#include "prom_metric.h"

/**
 * @brief A prometheus gauge.
 *
 * References
 * * See https://prometheus.io/docs/concepts/metric_types/#gauge
 */
typedef prom_metric_t prom_gauge_t;

/**
 * @brief Constructs a prom_gauge_t*
 * @param name The name of the metric
 * @param help The metric description
 * @param label_key_count The number of labels associated with the given metric. Pass 0 if the metric does not
 *                        require labels.
 * @param label_keys A collection of label keys. The number of keys MUST match the value passed as label_key_count. If
 *                   no labels are required, pass NULL. Otherwise, it may be convenient to pass this value as a
 *                   literal.
 * @return The constructed prom_guage_t*
 *
 *     // An example with labels
 *     prom_gauge_new("foo", "foo is a gauge with labels", 2, (const char**) { "one", "two" });
 *
 *     // An example without labels
 *     prom_gauge_new("foo", "foo is a gauge without labels", 0, NULL);
 */
prom_gauge_t *prom_gauge_new(const char *name, const char *help, size_t label_key_count, const char **label_keys);

/**
 * @brief Destroys a prom_gauge_t*. You must set self to NULL after destruction. A non-zero integer value will be
 *        returned on failure.
 * @param self The target prom_gauge_t*
 * @return A non-zero integer value upon failure
 */
int prom_gauge_destroy(prom_gauge_t *self);

/**
 * @brief Increment the prom_gauge_t* by 1.
 * @param self The target  prom_gauger_t*
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the gauge's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return A non-zero integer value upon failure
 * *Example*
 *
 *     // An example with labels
 *     prom_gauge_inc(foo_gauge, (const char**) { "bar", "bang" });
 *
 *     // An example without labels
 *     prom_gauge_inc(foo_gauge, NULL);
 */
int prom_gauge_inc(prom_gauge_t *self, const char **label_values);

/**
 * @brief Decrement the prom_gauge_t* by 1.
 * @param self The target  prom_gauger_t*
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the gauge's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return A non-zero integer value upon failure.
 * *Example*
 *
 *     // An example with labels
 *     prom_gauge_dec(foo_gauge, (const char**) { "bar", "bang" });
 *
 *     // An example without labels
 *     prom_gauge_dec(foo_gauge, NULL);
 */
int prom_gauge_dec(prom_gauge_t *self, const char **label_values);

/**
 * @brief Add the value to the prom_gauge_t*.
 * @param self The target prom_gauge_t*
 * @param r_value The double to add to the prom_gauge_t passed as self.
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the gauge's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return A non-zero integer value upon failure.
 *
 * *Example*
 *
 *     // An example with labels
 *     prom_gauge_add(foo_gauge 22, (const char**) { "bar", "bang" });
 *
 *     // An example without labels
 *     prom_gauge_add(foo_gauge, 22, NULL);
 */
int prom_gauge_add(prom_gauge_t *self, double r_value, const char **label_values);

/**
 * @brief Subtract the value to the prom_gauge. A non-zero integer value will be returned on failure.
 * @param self The target prom_gauge_t*
 * @param r_value The double to add to the prom_gauge_t passed as self.
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the gauge's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return A non-zero integer value upon failure.
 *
 * *Example*
 *
 *     // An example with labels
 *     prom_gauge_sub(foo_gauge 22, (const char**) { "bar", "bang" });
 *
 *     // An example without labels
 *     prom_gauge_sub(foo_gauge, 22, NULL);
 */
int prom_gauge_sub(prom_gauge_t *self, double r_value, const char **label_values);

/**
 * @brief Set the value for the prom_gauge_t*
 * @param self The target prom_gauge_t*
 * @param r_value The double to which the prom_gauge_t* passed as self will be set
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the gauge's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return A non-zero integer value upon failure.
 *
 * *Example*
 *
 *     // An example with labels
 *     prom_gauge_set(foo_gauge 22, (const char**) { "bar", "bang" });
 *
 *     // An example without labels
 *     prom_gauge_set(foo_gauge, 22, NULL);
 */
int prom_gauge_set(prom_gauge_t *self, double r_value, const char **label_values);

#endif  // PROM_GAUGE_H

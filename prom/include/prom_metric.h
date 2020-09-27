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
 * @file prom_metric.h
 * @brief Functions for retrieving metric samples from metrics given an ordered set of labels
 */

#ifndef PROM_METRIC_H
#define PROM_METRIC_H

#include "prom_metric_sample.h"
#include "prom_metric_sample_histogram.h"

struct prom_metric;
/**
 * @brief A prometheus metric.
 *
 * Reference: https://prometheus.io/docs/concepts/data_model
 */
typedef struct prom_metric prom_metric_t;

/**
 * @brief Returns a prom_metric_sample_t*. The order of label_values is significant.
 *
 * You may use this function to cache metric samples to avoid sample lookup. Metric samples are stored in a hash map
 * with O(1) lookups in average case; nonethless, caching metric samples and updating them directly might be
 * preferrable in performance-sensitive situations.
 *
 * @param self The target prom_metric_t*
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the counter's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return A prom_metric_sample_t*
 */
prom_metric_sample_t *prom_metric_sample_from_labels(prom_metric_t *self, const char **label_values);

/**
 * @brief Returns a prom_metric_sample_histogram_t*. The order of label_values is significant.
 *
 * You may use this function to cache metric samples to avoid sample lookup. Metric samples are stored in a hash map
 * with O(1) lookups in average case; nonethless, caching metric samples and updating them directly might be
 * preferrable in performance-sensitive situations.
 *
 * @param self The target prom_histogram_metric_t*
 * @param label_values The label values associated with the metric sample being updated. The number of labels must
 *                     match the value passed to label_key_count in the counter's constructor. If no label values are
 *                     necessary, pass NULL. Otherwise, It may be convenient to pass this value as a literal.
 * @return prom_metric_sample_histogram_t*
 */
prom_metric_sample_histogram_t *prom_metric_sample_histogram_from_labels(prom_metric_t *self,
                                                                         const char **label_values);

#endif  // PROM_METRIC_H

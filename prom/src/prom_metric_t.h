/**
 * Copyright 2019-2020 DigitalOcean Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PROM_METRIC_T_H
#define PROM_METRIC_T_H

#include <pthread.h>

// Public
#include "prom_histogram_buckets.h"
#include "prom_metric.h"

// Private
#include "prom_map_i.h"
#include "prom_map_t.h"
#include "prom_metric_formatter_t.h"

/**
 * @brief API PRIVATE Contains metric type constants
 */
typedef enum prom_metric_type { PROM_COUNTER, PROM_GAUGE, PROM_HISTOGRAM, PROM_SUMMARY } prom_metric_type_t;

/**
 * @brief API PRIVATE Maps metric type constants to human readable string values
 */
extern char *prom_metric_type_map[4];

/**
 * @brief API PRIVATE An opaque struct to users containing metric metadata; one or more metric samples; and a metric
 * formatter for locating metric samples and exporting metric data
 */
struct prom_metric {
  prom_metric_type_t type;            /**< metric_type      The type of metric */
  const char *name;                   /**< name             The name of the metric */
  const char *help;                   /**< help             The help output for the metric */
  prom_map_t *samples;                /**< samples          Map comprised of samples for the given metric */
  prom_histogram_buckets_t *buckets;  /**< buckets          Array of histogram bucket upper bound values */
  size_t label_key_count;             /**< label_keys_count The count of labe_keys*/
  prom_metric_formatter_t *formatter; /**< formatter        The metric formatter  */
  pthread_rwlock_t *rwlock;           /**< rwlock           Required for locking on certain non-atomic operations */
  const char **label_keys;            /**< labels           Array comprised of const char **/
};

#endif  // PROM_METRIC_T_H

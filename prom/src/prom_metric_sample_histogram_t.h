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

#include <pthread.h>

// Public
#include "prom_histogram_buckets.h"
#include "prom_metric_sample_histogram.h"

// Private
#include "prom_map_t.h"
#include "prom_metric_formatter_t.h"

#ifndef PROM_METRIC_HISTOGRAM_SAMPLE_T_H
#define PROM_METRIC_HISTOGRAM_SAMPLE_T_H

struct prom_metric_sample_histogram {
  prom_linked_list_t *l_value_list;
  prom_map_t *l_values;
  prom_map_t *samples;
  prom_metric_formatter_t *metric_formatter;
  prom_histogram_buckets_t *buckets;
  pthread_rwlock_t *rwlock;
};

#endif  // PROM_METRIC_HISTOGRAM_SAMPLE_T_H

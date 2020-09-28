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

#ifndef PROM_METRIC_HISTOGRAM_SAMPLE_I_H
#define PROM_METRIC_HISTOGRAM_SAMPLE_I_H

// Public
#include "prom_metric_sample_histogram.h"

// Private
#include "prom_metric_sample_histogram_t.h"

/**
 * @brief API PRIVATE Create a pointer to a prom_metric_sample_histogram_t
 */
prom_metric_sample_histogram_t *prom_metric_sample_histogram_new(const char *name, prom_histogram_buckets_t *buckets,
                                                                 size_t label_count, const char **label_keys,
                                                                 const char **label_vales);

/**
 * @brief API PRIVATE Destroy a prom_metric_sample_histogram_t
 */
int prom_metric_sample_histogram_destroy(prom_metric_sample_histogram_t *self);

/**
 * @brief API PRIVATE Destroy a void pointer that is cast to a prom_metric_sample_histogram_t*
 */
int prom_metric_sample_histogram_destroy_generic(void *gen);

char *prom_metric_sample_histogram_bucket_to_str(double bucket);

void prom_metric_sample_histogram_free_generic(void *gen);

#endif  // PROM_METRIC_HISTOGRAM_SAMPLE_I_H

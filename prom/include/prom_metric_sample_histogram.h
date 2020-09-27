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
 * @file prom_metric_sample_histogram.h
 * @brief Functions for interacting with histogram metric samples directly
 */

#ifndef PROM_METRIC_SAMPLE_HISOTGRAM_H
#define PROM_METRIC_SAMPLE_HISOTGRAM_H

struct prom_metric_sample_histogram;
/**
 * @brief A histogram metric sample
 */
typedef struct prom_metric_sample_histogram prom_metric_sample_histogram_t;

/**
 * @brief Observe the double for the given prom_metric_sample_histogram_observe_t
 * @param self The target prom_metric_sample_histogram_t*
 * @param value The value to observe.
 * @return Non-zero integer value upon failure
 */
int prom_metric_sample_histogram_observe(prom_metric_sample_histogram_t *self, double value);

#endif  // PROM_METRIC_SAMPLE_HISOTGRAM_H

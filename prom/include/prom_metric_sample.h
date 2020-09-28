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
 * @file prom_metric_sample.h
 * @brief Functions for interfacting with metric samples directly
 */

#ifndef PROM_METRIC_SAMPLE_H
#define PROM_METRIC_SAMPLE_H

struct prom_metric_sample;
/**
 * @brief Contains the specific metric and value given the name and label set
 * Reference: https://prometheus.io/docs/concepts/data_model/#metric-names-and-labels
 */
typedef struct prom_metric_sample prom_metric_sample_t;

/**
 * @brief Add the r_value to the sample. The value must be greater than or equal to zero.
 * @param self The target prom_metric_sample_t*
 * @param r_value The double to add to prom_metric_sample_t* provided by self
 * @return Non-zero integer value upon failure
 */
int prom_metric_sample_add(prom_metric_sample_t *self, double r_value);

/**
 * @brief Subtract the r_value from the sample.
 *
 * This operation MUST be called a sample derived from a gauge metric.
 * @param self The target prom_metric_sample_t*
 * @param r_value The double to subtract from the prom_metric_sample_t* provided by self
 * @return Non-zero integer value upon failure
 */
int prom_metric_sample_sub(prom_metric_sample_t *self, double r_value);

/**
 * @brief Set the r_value of the sample.
 *
 * This operation MUST be called on a sample derived from a gauge metric.
 * @param self The target prom_metric_sample_t*
 * @param r_value The double which will be set to the prom_metric_sample_t* provided by self
 * @return Non-zero integer value upon failure
 */
int prom_metric_sample_set(prom_metric_sample_t *self, double r_value);

#endif  // PROM_METRIC_SAMPLE_H

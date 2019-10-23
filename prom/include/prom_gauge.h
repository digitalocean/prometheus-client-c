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
 * @file prom_gauge.h
 * @brief https://prometheus.io/docs/concepts/metric_types/#gauge
 */

#ifndef PROM_GAUGE_INCLUDED
#define PROM_GAUGE_INCLUDED

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
 * @brief Returns a pointer to a prom_gauge
 */
prom_gauge_t* prom_gauge_new(const char *name,
                             const char *help,
                             size_t label_key_count,
                             const char **label_keys);

/**
 * @brief Destroys a pointer to a prom_gauge
 */
int prom_gauge_destroy(prom_gauge_t *self);


/**
 * @brief Increment the prom_gauge by 1
 */
int prom_gauge_inc(prom_gauge_t *self, const char **label_values);


/**
 * @brief Descrement the prom_gauge by 1
 */
int prom_gauge_dec(prom_gauge_t *self, const char **label_values);


/**
 * @brief Add the value to the prom_gauge
 */
int prom_gauge_add(prom_gauge_t *self, double r_value, const char **label_values);


/**
 * @brief Subtract the value from the prom_gauge
 */
int prom_gauge_sub(prom_gauge_t *self, double r_value, const char **label_values);


/**
 * @brief Set the value for the prom_gauge
 */
int prom_gauge_set(prom_gauge_t *self, double r_value, const char **label_values);

#endif  // PROM_GAUGE_INCLUDED

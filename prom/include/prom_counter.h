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


#ifndef PROM_COUNTER_INCLUDED
#define PROM_COUNTER_INCLUDED

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
 * @brief Returns a pointer to a prom_counter
 */
prom_counter_t* prom_counter_new(const char *name,
                                 const char *help,
                                 size_t label_key_count,
                                 const char **label_keys);

/**
 * @brief Destroys a pointer to a prom_counter
 */
int prom_counter_destroy(prom_counter_t *self);


/**
 * @brief Increment the prom_counter by 1
 */
int prom_counter_inc(prom_counter_t *self, const char **label_values);


/**
 * @brief Add the value to the prom_counter
 */
int prom_counter_add(prom_counter_t *self, double r_value, const char **label_values);

#endif  // PROM_COUNTER_INCLUDED
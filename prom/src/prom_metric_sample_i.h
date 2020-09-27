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

#include "prom_metric_sample_t.h"
#include "prom_metric_t.h"

#ifndef PROM_METRIC_SAMPLE_I_H
#define PROM_METRIC_SAMPLE_I_H

/**
 * @brief API PRIVATE Return a prom_metric_sample_t*
 *
 * @param type The type of metric sample
 * @param l_value The entire left value of the metric e.g metric_name{foo="bar"}
 * @param r_value A double representing the value of the sample
 */
prom_metric_sample_t *prom_metric_sample_new(prom_metric_type_t type, const char *l_value, double r_value);

/**
 * @brief API PRIVATE Destroy the prom_metric_sample**
 */
int prom_metric_sample_destroy(prom_metric_sample_t *self);

/**
 * @brief API PRIVATE A prom_linked_list_free_item_fn to enable item destruction within a linked list's destructor
 */
int prom_metric_sample_destroy_generic(void *);

/**
 * @brief API PRIVATE A prom_linked_list_free_item_fn to enable item destruction within a linked list's destructor.
 *
 * This function ignores any errors.
 */
void prom_metric_sample_free_generic(void *gen);

#endif  // PROM_METRIC_SAMPLE_I_H

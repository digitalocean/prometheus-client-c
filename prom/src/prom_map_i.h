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

#ifndef PROM_MAP_I_INCLUDED
#define PROM_MAP_I_INCLUDED

#include "prom_map_t.h"

prom_map_t *prom_map_new(void);

int prom_map_set_free_value_fn(prom_map_t *self, prom_map_node_free_value_fn free_value_fn);

void *prom_map_get(prom_map_t *self, const char *key);

int prom_map_set(prom_map_t *self, const char *key, void *value);

int prom_map_delete(prom_map_t *self, const char *key);

int prom_map_destroy(prom_map_t *self);

size_t prom_map_size(prom_map_t *self);

prom_map_node_t *prom_map_node_new(const char *key, void *value, prom_map_node_free_value_fn free_value_fn);

#endif  // PROM_MAP_I_INCLUDED

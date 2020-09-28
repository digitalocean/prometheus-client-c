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

#ifndef PROM_REGISTRY_T_H
#define PROM_REGISTRY_T_H

#include <pthread.h>
#include <stdbool.h>

// Public
#include "prom_collector_registry.h"

// Private
#include "prom_map_t.h"
#include "prom_metric_formatter_t.h"
#include "prom_string_builder_t.h"

struct prom_collector_registry {
  const char *name;
  bool disable_process_metrics;              /**< Disables the collection of process metrics */
  prom_map_t *collectors;                    /**< Map of collectors keyed by name */
  prom_string_builder_t *string_builder;     /**< Enables string building */
  prom_metric_formatter_t *metric_formatter; /**< metric formatter for metric exposition on bridge call */
  pthread_rwlock_t *lock;                    /**< mutex for safety against concurrent registration */
};

#endif  // PROM_REGISTRY_T_H

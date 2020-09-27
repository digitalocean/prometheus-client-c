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

#ifndef PROM_COLLECTOR_T_H
#define PROM_COLLECTOR_T_H

#include "prom_collector.h"
#include "prom_map_t.h"
#include "prom_string_builder_t.h"

struct prom_collector {
  const char *name;
  prom_map_t *metrics;
  prom_collect_fn *collect_fn;
  prom_string_builder_t *string_builder;
  const char *proc_limits_file_path;
  const char *proc_stat_file_path;
};

#endif  // PROM_COLLECTOR_T_H

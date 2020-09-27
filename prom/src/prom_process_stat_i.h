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

#ifndef PROM_PROCESS_STATS_I_H
#define PROM_PROCESS_STATS_I_H

#include "prom_process_stat_t.h"

prom_process_stat_file_t *prom_process_stat_file_new(const char *path);
int prom_process_stat_file_destroy(prom_process_stat_file_t *self);
prom_process_stat_t *prom_process_stat_new(prom_process_stat_file_t *stat_f);
int prom_process_stat_destroy(prom_process_stat_t *self);
int prom_process_stats_init(void);

#endif  // PROM_PROCESS_STATS_I_H

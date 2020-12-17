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

#ifndef PROM_PROCESS_T_H
#define PROM_PROCESS_T_H

#include "prom_gauge.h"
#include "prom_procfs_t.h"

extern prom_gauge_t *prom_process_open_fds;
extern prom_gauge_t *prom_process_max_fds;
extern prom_gauge_t *prom_process_virtual_memory_max_bytes;

typedef struct prom_process_limits_row {
  const char *limit; /**< Pointer to a string */
  int soft;          /**< Soft value */
  int hard;          /**< Hard value */
  const char *units; /**< Units  */
} prom_process_limits_row_t;

typedef struct prom_process_limits_current_row {
  char *limit; /**< Pointer to a string */
  int soft;    /**< Soft value */
  int hard;    /**< Hard value */
  char *units; /**< Units  */
} prom_process_limits_current_row_t;

typedef prom_procfs_buf_t prom_process_limits_file_t;

#endif  // PROM_PROCESS_T_H

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

#include <stdio.h>
#include <unistd.h>

// Public
#include "prom_alloc.h"
#include "prom_collector.h"
#include "prom_collector_registry.h"

// Private
#include "prom_assert.h"
#include "prom_collector_t.h"
#include "prom_log.h"
#include "prom_map_i.h"
#include "prom_metric_i.h"
#include "prom_process_fds_i.h"
#include "prom_process_fds_t.h"
#include "prom_process_limits_i.h"
#include "prom_process_limits_t.h"
#include "prom_process_stat_i.h"
#include "prom_process_stat_t.h"
#include "prom_string_builder_i.h"

prom_map_t *prom_collector_default_collect(prom_collector_t *self) { return self->metrics; }

prom_collector_t *prom_collector_new(const char *name) {
  int r = 0;
  prom_collector_t *self = (prom_collector_t *)prom_malloc(sizeof(prom_collector_t));
  self->name = prom_strdup(name);
  self->metrics = prom_map_new();
  if (self->metrics == NULL) {
    prom_collector_destroy(self);
    return NULL;
  }
  r = prom_map_set_free_value_fn(self->metrics, &prom_metric_free_generic);
  if (r) {
    prom_collector_destroy(self);
    return NULL;
  }
  self->collect_fn = &prom_collector_default_collect;
  self->string_builder = prom_string_builder_new();
  if (self->string_builder == NULL) {
    prom_collector_destroy(self);
    return NULL;
  }
  self->proc_limits_file_path = NULL;
  self->proc_stat_file_path = NULL;
  return self;
}

int prom_collector_destroy(prom_collector_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 0;

  int r = 0;
  int ret = 0;

  r = prom_map_destroy(self->metrics);
  if (r) ret = r;
  self->metrics = NULL;

  r = prom_string_builder_destroy(self->string_builder);
  if (r) ret = r;
  self->string_builder = NULL;

  prom_free((char *)self->name);
  self->name = NULL;
  prom_free(self);
  self = NULL;

  return ret;
}

int prom_collector_destroy_generic(void *gen) {
  int r = 0;
  prom_collector_t *self = (prom_collector_t *)gen;
  r = prom_collector_destroy(self);
  self = NULL;
  return r;
}

void prom_collector_free_generic(void *gen) {
  prom_collector_t *self = (prom_collector_t *)gen;
  prom_collector_destroy(self);
}

int prom_collector_set_collect_fn(prom_collector_t *self, prom_collect_fn *fn) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  self->collect_fn = fn;
  return 0;
}

int prom_collector_add_metric(prom_collector_t *self, prom_metric_t *metric) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  if (prom_map_get(self->metrics, metric->name) != NULL) {
    PROM_LOG("metric already found in collector");
    return 1;
  }
  return prom_map_set(self->metrics, metric->name, metric);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Process Collector

prom_map_t *prom_collector_process_collect(prom_collector_t *self);

prom_collector_t *prom_collector_process_new(const char *limits_path, const char *stat_path) {
  prom_collector_t *self = prom_collector_new("process");
  PROM_ASSERT(self != NULL);
  if (self == NULL) return NULL;

  int r = 0;

  self->proc_limits_file_path = limits_path;
  self->proc_stat_file_path = stat_path;
  self->collect_fn = &prom_collector_process_collect;

  r = prom_process_limits_init();
  if (r) return NULL;

  r = prom_process_stats_init();
  if (r) return NULL;

  r = prom_process_fds_init();
  if (r) return NULL;

  r = prom_collector_add_metric(self, prom_process_max_fds);
  if (r) return NULL;

  r = prom_collector_add_metric(self, prom_process_virtual_memory_max_bytes);
  if (r) return NULL;

  r = prom_collector_add_metric(self, prom_process_cpu_seconds_total);
  if (r) return NULL;

  r = prom_collector_add_metric(self, prom_process_virtual_memory_bytes);
  if (r) return NULL;

  r = prom_collector_add_metric(self, prom_process_resident_memory_bytes);
  if (r) return NULL;

  r = prom_collector_add_metric(self, prom_process_start_time_seconds);
  if (r) return NULL;

  r = prom_collector_add_metric(self, prom_process_open_fds);
  if (r) return NULL;

  return self;
}

prom_map_t *prom_collector_process_collect(prom_collector_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return NULL;

  int r = 0;

  // Allocate and create a *prom_process_limits_file_t
  prom_process_limits_file_t *limits_f = prom_process_limits_file_new(self->proc_limits_file_path);
  if (limits_f == NULL) {
    prom_process_limits_file_destroy(limits_f);
    return NULL;
  }

  // Allocate and create a *prom_map_t from prom_process_limits_file_t. This is the main storage container for the
  // limits metric data
  prom_map_t *limits_map = prom_process_limits(limits_f);
  if (limits_map == NULL) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    return NULL;
  }

  // Retrieve the *prom_process_limits_row_t for Max open files
  prom_process_limits_row_t *max_fds = (prom_process_limits_row_t *)prom_map_get(limits_map, "Max open files");
  if (max_fds == NULL) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    return NULL;
  }

  // Retrieve the *prom_process_limits_row_t for Max address space
  prom_process_limits_row_t *virtual_memory_max_bytes =
      (prom_process_limits_row_t *)prom_map_get(limits_map, "Max address space");
  if (virtual_memory_max_bytes == NULL) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    return NULL;
  }

  // Set the metric values for max_fds and virtual_memory_max_bytes
  r = prom_gauge_set(prom_process_max_fds, max_fds->soft, NULL);
  if (r) return NULL;
  r = prom_gauge_set(prom_process_virtual_memory_max_bytes, virtual_memory_max_bytes->soft, NULL);
  if (r) return NULL;

  // Aloocate and create a *prom_process_stat_file_t
  prom_process_stat_file_t *stat_f = prom_process_stat_file_new(self->proc_stat_file_path);
  if (stat_f == NULL) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    return self->metrics;
  }

  // Allocate and create a *prom_process_stat_t from *prom_process_stat_file_t
  prom_process_stat_t *stat = prom_process_stat_new(stat_f);

  // Set the metrics related to the stat file
  r = prom_gauge_set(prom_process_cpu_seconds_total, ((stat->utime + stat->stime) / sysconf(_SC_CLK_TCK)), NULL);
  if (r) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    prom_process_stat_file_destroy(stat_f);
    prom_process_stat_destroy(stat);
    return NULL;
  }
  r = prom_gauge_set(prom_process_virtual_memory_bytes, stat->vsize, NULL);
  if (r) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    prom_process_stat_file_destroy(stat_f);
    prom_process_stat_destroy(stat);
    return NULL;
  }
  r = prom_gauge_set(prom_process_resident_memory_bytes, stat->rss*sysconf(_SC_PAGE_SIZE), NULL);
  if (r) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    prom_process_stat_file_destroy(stat_f);
    prom_process_stat_destroy(stat);
    return NULL;
  }
  r = prom_gauge_set(prom_process_start_time_seconds, stat->starttime, NULL);
  if (r) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    prom_process_stat_file_destroy(stat_f);
    prom_process_stat_destroy(stat);
    return NULL;
  }
  r = prom_gauge_set(prom_process_open_fds, prom_process_fds_count(NULL), NULL);
  if (r) {
    prom_process_limits_file_destroy(limits_f);
    prom_map_destroy(limits_map);
    prom_process_stat_file_destroy(stat_f);
    prom_process_stat_destroy(stat);
    return NULL;
  }

  // If there is any issue deallocating the following structures, return NULL to indicate failure
  r = prom_process_limits_file_destroy(limits_f);
  if (r) return NULL;
  r = prom_map_destroy(limits_map);
  if (r) return NULL;
  r = prom_process_stat_file_destroy(stat_f);
  if (r) return NULL;
  r = prom_process_stat_destroy(stat);
  if (r) return NULL;

  return self->metrics;
}

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

#include <pthread.h>
#include <regex.h>
#include <stdio.h>

// Public
#include "prom_alloc.h"
#include "prom_collector.h"
#include "prom_collector_registry.h"

// Private
#include "prom_assert.h"
#include "prom_collector_registry_t.h"
#include "prom_collector_t.h"
#include "prom_errors.h"
#include "prom_log.h"
#include "prom_map_i.h"
#include "prom_metric_formatter_i.h"
#include "prom_metric_i.h"
#include "prom_metric_t.h"
#include "prom_process_limits_i.h"
#include "prom_string_builder_i.h"

prom_collector_registry_t *PROM_COLLECTOR_REGISTRY_DEFAULT;

prom_collector_registry_t *prom_collector_registry_new(const char *name) {
  int r = 0;

  prom_collector_registry_t *self = (prom_collector_registry_t *)prom_malloc(sizeof(prom_collector_registry_t));

  self->disable_process_metrics = false;

  self->name = prom_strdup(name);
  self->collectors = prom_map_new();
  prom_map_set_free_value_fn(self->collectors, &prom_collector_free_generic);
  prom_map_set(self->collectors, "default", prom_collector_new("default"));

  self->metric_formatter = prom_metric_formatter_new();
  self->string_builder = prom_string_builder_new();
  self->lock = (pthread_rwlock_t *)prom_malloc(sizeof(pthread_rwlock_t));
  r = pthread_rwlock_init(self->lock, NULL);
  if (r) {
    PROM_LOG("failed to initialize rwlock");
    return NULL;
  }
  return self;
}

int prom_collector_registry_enable_process_metrics(prom_collector_registry_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  prom_collector_t *process_collector = prom_collector_process_new(NULL, NULL);
  if (process_collector) {
    prom_map_set(self->collectors, "process", process_collector);
    return 0;
  }
  return 1;
}

int prom_collector_registry_enable_custom_process_metrics(prom_collector_registry_t *self,
                                                          const char *process_limits_path,
                                                          const char *process_stats_path) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) {
    PROM_LOG("prom_collector_registry_t is NULL");
    return 1;
  }
  prom_collector_t *process_collector = prom_collector_process_new(process_limits_path, process_stats_path);
  if (process_collector) {
    prom_map_set(self->collectors, "process", process_collector);
    return 0;
  }
  return 1;
}

int prom_collector_registry_default_init(void) {
  if (PROM_COLLECTOR_REGISTRY_DEFAULT != NULL) return 0;

  PROM_COLLECTOR_REGISTRY_DEFAULT = prom_collector_registry_new("default");
  if (PROM_COLLECTOR_REGISTRY_DEFAULT) {
    return prom_collector_registry_enable_process_metrics(PROM_COLLECTOR_REGISTRY_DEFAULT);
  }
  return 1;
}

int prom_collector_registry_destroy(prom_collector_registry_t *self) {
  if (self == NULL) return 0;

  int r = 0;
  int ret = 0;

  r = prom_map_destroy(self->collectors);
  self->collectors = NULL;
  if (r) ret = r;

  r = prom_metric_formatter_destroy(self->metric_formatter);
  self->metric_formatter = NULL;
  if (r) ret = r;

  r = prom_string_builder_destroy(self->string_builder);
  self->string_builder = NULL;
  if (r) ret = r;

  r = pthread_rwlock_destroy(self->lock);
  prom_free(self->lock);
  self->lock = NULL;
  if (r) ret = r;

  prom_free((char *)self->name);
  self->name = NULL;

  prom_free(self);
  self = NULL;

  return ret;
}

int prom_collector_registry_register_metric(prom_metric_t *metric) {
  PROM_ASSERT(metric != NULL);

  prom_collector_t *default_collector =
      (prom_collector_t *)prom_map_get(PROM_COLLECTOR_REGISTRY_DEFAULT->collectors, "default");

  if (default_collector == NULL) {
    return 1;
  }

  return prom_collector_add_metric(default_collector, metric);
}

prom_metric_t *prom_collector_registry_must_register_metric(prom_metric_t *metric) {
  int err = prom_collector_registry_register_metric(metric);
  if (err != 0) {
    exit(err);
  }
  return metric;
}

int prom_collector_registry_register_collector(prom_collector_registry_t *self, prom_collector_t *collector) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;

  int r = 0;

  r = pthread_rwlock_wrlock(self->lock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_LOCK_ERROR);
    return 1;
  }
  if (prom_map_get(self->collectors, collector->name) != NULL) {
    PROM_LOG("the given prom_collector_t* is already registered");
    int rr = pthread_rwlock_unlock(self->lock);
    if (rr) {
      PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);
      return rr;
    } else {
      return 1;
    }
  }
  r = prom_map_set(self->collectors, collector->name, collector);
  if (r) {
    int rr = pthread_rwlock_unlock(self->lock);
    if (rr) {
      PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);
      return rr;
    } else {
      return r;
    }
  }
  r = pthread_rwlock_unlock(self->lock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);
    return 1;
  }
  return 0;
}

int prom_collector_registry_validate_metric_name(prom_collector_registry_t *self, const char *metric_name) {
  regex_t r;
  int ret = 0;
  ret = regcomp(&r, "^[a-zA-Z_:][a-zA-Z0-9_:]*$", REG_EXTENDED);
  if (ret) {
    PROM_LOG(PROM_REGEX_REGCOMP_ERROR);
    regfree(&r);
    return ret;
  }

  ret = regexec(&r, metric_name, 0, NULL, 0);
  if (ret) {
    PROM_LOG(PROM_REGEX_REGEXEC_ERROR);
    regfree(&r);
    return ret;
  }
  regfree(&r);
  return 0;
}

const char *prom_collector_registry_bridge(prom_collector_registry_t *self) {
  prom_metric_formatter_clear(self->metric_formatter);
  prom_metric_formatter_load_metrics(self->metric_formatter, self->collectors);
  return (const char *)prom_metric_formatter_dump(self->metric_formatter);
}

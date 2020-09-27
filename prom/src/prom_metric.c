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

// Public
#include "prom_alloc.h"
#include "prom_histogram_buckets.h"

// Private
#include "prom_assert.h"
#include "prom_errors.h"
#include "prom_log.h"
#include "prom_map_i.h"
#include "prom_metric_formatter_i.h"
#include "prom_metric_i.h"
#include "prom_metric_sample_histogram_i.h"
#include "prom_metric_sample_i.h"

char *prom_metric_type_map[4] = {"counter", "gauge", "histogram", "summary"};

prom_metric_t *prom_metric_new(prom_metric_type_t metric_type, const char *name, const char *help,
                               size_t label_key_count, const char **label_keys) {
  int r = 0;
  prom_metric_t *self = (prom_metric_t *)prom_malloc(sizeof(prom_metric_t));
  self->type = metric_type;
  self->name = name;
  self->help = help;
  self->buckets = NULL;

  const char **k = (const char **)prom_malloc(sizeof(const char *) * label_key_count);

  for (int i = 0; i < label_key_count; i++) {
    if (strcmp(label_keys[i], "le") == 0) {
      PROM_LOG(PROM_METRIC_INVALID_LABEL_NAME);
      prom_metric_destroy(self);
      return NULL;
    }
    if (strcmp(label_keys[i], "quantile") == 0) {
      PROM_LOG(PROM_METRIC_INVALID_LABEL_NAME);
      prom_metric_destroy(self);
      return NULL;
    }
    k[i] = prom_strdup(label_keys[i]);
  }
  self->label_keys = k;
  self->label_key_count = label_key_count;
  self->samples = prom_map_new();

  if (metric_type == PROM_HISTOGRAM) {
    r = prom_map_set_free_value_fn(self->samples, &prom_metric_sample_histogram_free_generic);
    if (r) {
      prom_metric_destroy(self);
      return NULL;
    }
  } else {
    r = prom_map_set_free_value_fn(self->samples, &prom_metric_sample_free_generic);
    if (r) {
      prom_metric_destroy(self);
      return NULL;
    }
  }

  self->formatter = prom_metric_formatter_new();
  if (self->formatter == NULL) {
    prom_metric_destroy(self);
    return NULL;
  }
  self->rwlock = (pthread_rwlock_t *)prom_malloc(sizeof(pthread_rwlock_t));
  r = pthread_rwlock_init(self->rwlock, NULL);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_INIT_ERROR);
    return NULL;
  }
  return self;
}

int prom_metric_destroy(prom_metric_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 0;

  int r = 0;
  int ret = 0;

  if (self->buckets != NULL) {
    r = prom_histogram_buckets_destroy(self->buckets);
    self->buckets = NULL;
    if (r) ret = r;
  }

  r = prom_map_destroy(self->samples);
  self->samples = NULL;
  if (r) ret = r;

  r = prom_metric_formatter_destroy(self->formatter);
  self->formatter = NULL;
  if (r) ret = r;

  r = pthread_rwlock_destroy(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_DESTROY_ERROR);
    ret = r;
  }

  prom_free(self->rwlock);
  self->rwlock = NULL;

  for (int i = 0; i < self->label_key_count; i++) {
    prom_free((void *)self->label_keys[i]);
    self->label_keys[i] = NULL;
  }
  prom_free(self->label_keys);
  self->label_keys = NULL;

  prom_free(self);
  self = NULL;

  return ret;
}

int prom_metric_destroy_generic(void *item) {
  int r = 0;
  prom_metric_t *self = (prom_metric_t *)item;
  r = prom_metric_destroy(self);
  self = NULL;
  return r;
}

void prom_metric_free_generic(void *item) {
  prom_metric_t *self = (prom_metric_t *)item;
  prom_metric_destroy(self);
}

prom_metric_sample_t *prom_metric_sample_from_labels(prom_metric_t *self, const char **label_values) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  r = pthread_rwlock_wrlock(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_LOCK_ERROR);
    return NULL;
  }

#define PROM_METRIC_SAMPLE_FROM_LABELS_HANDLE_UNLOCK() \
  r = pthread_rwlock_unlock(self->rwlock);             \
  if (r) PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);   \
  return NULL;

  // Get l_value
  r = prom_metric_formatter_load_l_value(self->formatter, self->name, NULL, self->label_key_count, self->label_keys,
                                         label_values);
  if (r) {
    PROM_METRIC_SAMPLE_FROM_LABELS_HANDLE_UNLOCK();
  }

  // This must be freed before returning
  const char *l_value = prom_metric_formatter_dump(self->formatter);
  if (l_value == NULL) {
    PROM_METRIC_SAMPLE_FROM_LABELS_HANDLE_UNLOCK();
  }

  // Get sample
  prom_metric_sample_t *sample = (prom_metric_sample_t *)prom_map_get(self->samples, l_value);
  if (sample == NULL) {
    sample = prom_metric_sample_new(self->type, l_value, 0.0);
    r = prom_map_set(self->samples, l_value, sample);
    if (r) {
      PROM_METRIC_SAMPLE_FROM_LABELS_HANDLE_UNLOCK();
    }
  }
  pthread_rwlock_unlock(self->rwlock);
  prom_free((void *)l_value);
  return sample;
}

prom_metric_sample_histogram_t *prom_metric_sample_histogram_from_labels(prom_metric_t *self,
                                                                         const char **label_values) {
  PROM_ASSERT(self != NULL);

  int r = 0;
  r = pthread_rwlock_wrlock(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_LOCK_ERROR);
    return NULL;
  }

#define PROM_METRIC_SAMPLE_HISTOGRAM_FROM_LABELS_HANDLE_UNLOCK() \
  r = pthread_rwlock_unlock(self->rwlock);                       \
  if (r) {                                                       \
    PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);                  \
    return NULL;                                                 \
  }

  // Load the l_value
  r = prom_metric_formatter_load_l_value(self->formatter, self->name, NULL, self->label_key_count, self->label_keys,
                                         label_values);
  if (r) {
    PROM_METRIC_SAMPLE_HISTOGRAM_FROM_LABELS_HANDLE_UNLOCK();
  }

  // This must be freed before returning
  const char *l_value = prom_metric_formatter_dump(self->formatter);
  if (l_value == NULL) {
    PROM_METRIC_SAMPLE_HISTOGRAM_FROM_LABELS_HANDLE_UNLOCK();
  }

  // Get sample
  prom_metric_sample_histogram_t *sample = (prom_metric_sample_histogram_t *)prom_map_get(self->samples, l_value);
  if (sample == NULL) {
    sample = prom_metric_sample_histogram_new(self->name, self->buckets, self->label_key_count, self->label_keys,
                                              label_values);
    if (sample == NULL) {
      prom_free((void *)l_value);
      PROM_METRIC_SAMPLE_HISTOGRAM_FROM_LABELS_HANDLE_UNLOCK();
    }
    r = prom_map_set(self->samples, l_value, sample);
    if (r) {
      prom_free((void *)l_value);
      pthread_rwlock_unlock(self->rwlock);
      PROM_METRIC_SAMPLE_HISTOGRAM_FROM_LABELS_HANDLE_UNLOCK();
    }
  }
  pthread_rwlock_unlock(self->rwlock);
  prom_free((void *)l_value);
  return sample;
}

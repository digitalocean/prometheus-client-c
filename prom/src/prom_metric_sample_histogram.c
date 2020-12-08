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
#include <stdio.h>

// Public
#include "prom_alloc.h"
#include "prom_histogram.h"

// Private
#include "prom_assert.h"
#include "prom_errors.h"
#include "prom_linked_list_i.h"
#include "prom_log.h"
#include "prom_map_i.h"
#include "prom_metric_formatter_i.h"
#include "prom_metric_sample_histogram_i.h"
#include "prom_metric_sample_i.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *prom_metric_sample_histogram_l_value_for_bucket(prom_metric_sample_histogram_t *self,
                                                                   const char *name, size_t label_count,
                                                                   const char **label_keys, const char **label_values,
                                                                   double bucket);

static const char *prom_metric_sample_histogram_l_value_for_inf(prom_metric_sample_histogram_t *self, const char *name,
                                                                size_t label_count, const char **label_keys,
                                                                const char **label_values);

static void prom_metric_sample_histogram_free_str_generic(void *gen);

static int prom_metric_sample_histogram_init_bucket_samples(prom_metric_sample_histogram_t *self, const char *name,
                                                            size_t label_count, const char **label_keys,
                                                            const char **label_values);

static int prom_metric_sample_histogram_init_inf(prom_metric_sample_histogram_t *self, const char *name,
                                                 size_t label_count, const char **label_keys,
                                                 const char **label_values);

static int prom_metric_sample_histogram_init_count(prom_metric_sample_histogram_t *self, const char *name,
                                                   size_t label_count, const char **label_keys,
                                                   const char **label_values);

static int prom_metric_sample_histogram_init_summary(prom_metric_sample_histogram_t *self, const char *name,
                                                     size_t label_count, const char **label_keys,
                                                     const char **label_values);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// End static declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

prom_metric_sample_histogram_t *prom_metric_sample_histogram_new(const char *name, prom_histogram_buckets_t *buckets,
                                                                 size_t label_count, const char **label_keys,
                                                                 const char **label_values) {
  // Capture return codes
  int r = 0;

  // Allocate and set self
  prom_metric_sample_histogram_t *self =
      (prom_metric_sample_histogram_t *)prom_malloc(sizeof(prom_metric_sample_histogram_t));

  // Allocate and set the l_value_list
  self->l_value_list = prom_linked_list_new();
  if (self->l_value_list == NULL) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Allocate and set the metric formatter
  self->metric_formatter = prom_metric_formatter_new();
  if (self->metric_formatter == NULL) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Store map of l_value/prom_metric_sample_t
  self->samples = prom_map_new();
  if (self->samples == NULL) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Set the free value function on the samples map
  r = prom_map_set_free_value_fn(self->samples, &prom_metric_sample_free_generic);
  if (r) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Set a map of bucket: l_value
  self->l_values = prom_map_new();  // Store map of bucket/l_value
  if (self->l_values == NULL) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Set the free value function for thhe l_values map
  r = prom_map_set_free_value_fn(self->l_values, prom_metric_sample_histogram_free_str_generic);
  if (r) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  self->buckets = buckets;

  // Allocate and initialize the lock
  self->rwlock = (pthread_rwlock_t *)prom_malloc(sizeof(pthread_rwlock_t));
  r = pthread_rwlock_init(self->rwlock, NULL);
  if (r) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Allocate and initialize bucket metric samples
  r = prom_metric_sample_histogram_init_bucket_samples(self, name, label_count, label_keys, label_values);
  if (r) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Allocate and initialize the +Inf metric sample
  r = prom_metric_sample_histogram_init_inf(self, name, label_count, label_keys, label_values);
  if (r) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Allocate and initialize the count metric sample
  r = prom_metric_sample_histogram_init_count(self, name, label_count, label_keys, label_values);
  if (r) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // Add summary sample
  r = prom_metric_sample_histogram_init_summary(self, name, label_count, label_keys, label_values);
  if (r) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }

  // The value of nodes in this map will be simple prom_metric_sample pointers.
  r = prom_map_set_free_value_fn(self->samples, &prom_metric_sample_free_generic);
  if (r) {
    prom_metric_sample_histogram_destroy(self);
    return NULL;
  }
  return self;
}

static int prom_metric_sample_histogram_init_bucket_samples(prom_metric_sample_histogram_t *self, const char *name,
                                                            size_t label_count, const char **label_keys,
                                                            const char **label_values) {
  PROM_ASSERT(self);
  int r = 0;
  int bucket_count = prom_histogram_buckets_count(self->buckets);

  // For each bucket, create an prom_metric_sample_t with an appropriate l_value and default value of 0.0. The
  // l_value will contain the metric name, user labels, and finally, the le label and bucket value.
  for (int i = 0; i < bucket_count; i++) {
    const char *l_value = prom_metric_sample_histogram_l_value_for_bucket(self, name, label_count, label_keys,
                                                                          label_values, self->buckets->upper_bounds[i]);
    if (l_value == NULL) return 1;

    r = prom_linked_list_append(self->l_value_list, prom_strdup(l_value));
    if (r) return r;

    const char *bucket_key = prom_metric_sample_histogram_bucket_to_str(self->buckets->upper_bounds[i]);
    if (bucket_key == NULL) return 1;

    r = prom_map_set(self->l_values, bucket_key, (char *)l_value);
    if (r) return r;

    prom_metric_sample_t *sample = prom_metric_sample_new(PROM_HISTOGRAM, l_value, 0.0);
    if (sample == NULL) return 1;

    r = prom_map_set(self->samples, l_value, sample);
    if (r) return r;

    prom_free((void *)bucket_key);
  }
  return 0;
}

static int prom_metric_sample_histogram_init_inf(prom_metric_sample_histogram_t *self, const char *name,
                                                 size_t label_count, const char **label_keys,
                                                 const char **label_values) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  const char *inf_l_value =
      prom_metric_sample_histogram_l_value_for_inf(self, name, label_count, label_keys, label_values);
  if (inf_l_value == NULL) return 1;

  r = prom_linked_list_append(self->l_value_list, prom_strdup(inf_l_value));
  if (r) return r;

  r = prom_map_set(self->l_values, "+Inf", (char *)inf_l_value);
  if (r) return r;

  prom_metric_sample_t *inf_sample = prom_metric_sample_new(PROM_HISTOGRAM, (char *)inf_l_value, 0.0);
  if (inf_sample == NULL) return 1;

  return prom_map_set(self->samples, inf_l_value, inf_sample);
}

static int prom_metric_sample_histogram_init_count(prom_metric_sample_histogram_t *self, const char *name,
                                                   size_t label_count, const char **label_keys,
                                                   const char **label_values) {
  PROM_ASSERT(self != NULL);
  int r = 0;

  r = prom_metric_formatter_load_l_value(self->metric_formatter, name, "count", label_count, label_keys, label_values);
  if (r) return r;

  const char *count_l_value = prom_metric_formatter_dump(self->metric_formatter);
  if (count_l_value == NULL) return 1;

  r = prom_linked_list_append(self->l_value_list, prom_strdup(count_l_value));
  if (r) return r;

  r = prom_map_set(self->l_values, "count", (char *)count_l_value);
  if (r) return r;

  prom_metric_sample_t *count_sample = prom_metric_sample_new(PROM_HISTOGRAM, count_l_value, 0.0);
  if (count_sample == NULL) return 1;

  return prom_map_set(self->samples, count_l_value, count_sample);
}

static int prom_metric_sample_histogram_init_summary(prom_metric_sample_histogram_t *self, const char *name,
                                                     size_t label_count, const char **label_keys,
                                                     const char **label_values) {
  PROM_ASSERT(self != NULL);
  int r = 0;

  r = prom_metric_formatter_load_l_value(self->metric_formatter, name, "sum", label_count, label_keys, label_values);
  if (r) return r;

  const char *sum_l_value = prom_metric_formatter_dump(self->metric_formatter);
  if (sum_l_value == NULL) return 1;

  r = prom_linked_list_append(self->l_value_list, prom_strdup(sum_l_value));
  if (r) return r;

  r = prom_map_set(self->l_values, "sum", (char *)sum_l_value);
  if (r) return r;

  prom_metric_sample_t *sum_sample = prom_metric_sample_new(PROM_HISTOGRAM, sum_l_value, 0.0);
  if (sum_sample == NULL) return 1;

  return prom_map_set(self->samples, sum_l_value, sum_sample);
}

int prom_metric_sample_histogram_destroy(prom_metric_sample_histogram_t *self) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  int ret = 0;

  if (self == NULL) return 0;

  r = prom_linked_list_destroy(self->l_value_list);
  self->l_value_list = NULL;
  if (r) ret = r;

  r = prom_map_destroy(self->samples);
  if (r) ret = r;
  self->samples = NULL;

  r = prom_map_destroy(self->l_values);
  if (r) ret = r;
  self->l_values = NULL;

  r = prom_metric_formatter_destroy(self->metric_formatter);
  if (r) ret = r;
  self->metric_formatter = NULL;

  r = pthread_rwlock_destroy(self->rwlock);
  if (r) ret = r;

  prom_free(self->rwlock);
  self->rwlock = NULL;

  prom_free(self);
  self = NULL;
  return ret;
}

int prom_metric_sample_histogram_destroy_generic(void *gen) {
  int r = 0;

  prom_metric_sample_histogram_t *self = (prom_metric_sample_histogram_t *)gen;
  r = prom_metric_sample_histogram_destroy(self);
  self = NULL;
  return r;
}

void prom_metric_sample_histogram_free_generic(void *gen) {
  prom_metric_sample_histogram_t *self = (prom_metric_sample_histogram_t *)gen;
  prom_metric_sample_histogram_destroy(self);
}

int prom_metric_sample_histogram_observe(prom_metric_sample_histogram_t *self, double value) {
  int r = 0;

  r = pthread_rwlock_wrlock(self->rwlock);
  if (r) {
    printf("RETURN CODE: %d\n", r);
    PROM_LOG(PROM_PTHREAD_RWLOCK_LOCK_ERROR);
    return r;
  }

#define PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(r) \
  int rr = 0;                                                 \
  rr = pthread_rwlock_unlock(self->rwlock);                   \
  if (rr) {                                                   \
    PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);               \
    return rr;                                                \
  } else {                                                    \
    return r;                                                 \
  }

  // Update the counter for the proper bucket if found
  int bucket_count = prom_histogram_buckets_count(self->buckets);
  for (int i = (bucket_count - 1); i >= 0; i--) {
    if (value > self->buckets->upper_bounds[i]) {
      break;
    }

    const char *bucket_key = prom_metric_sample_histogram_bucket_to_str(self->buckets->upper_bounds[i]);
    if (bucket_key == NULL) {
      PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1)
    }

    const char *l_value = prom_map_get(self->l_values, bucket_key);
    if (l_value == NULL) {
      prom_free((void *)bucket_key);
      PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1)
    }

    prom_metric_sample_t *sample = prom_map_get(self->samples, l_value);
    if (sample == NULL) {
      prom_free((void *)bucket_key);
      PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1)
    }

    prom_free((void *)bucket_key);
    r = prom_metric_sample_add(sample, 1.0);
    if (r) {
      PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(r);
    }
  }

  // Update the +Inf and count samples
  const char *inf_l_value = prom_map_get(self->l_values, "+Inf");
  if (inf_l_value == NULL) {
    PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1);
  }

  prom_metric_sample_t *inf_sample = prom_map_get(self->samples, inf_l_value);
  if (inf_sample == NULL) {
    PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1);
  }

  r = prom_metric_sample_add(inf_sample, 1.0);
  if (r) {
    PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1);
  }

  const char *count_l_value = prom_map_get(self->l_values, "count");
  if (count_l_value == NULL) {
    PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1);
  }

  prom_metric_sample_t *count_sample = prom_map_get(self->samples, count_l_value);
  if (count_sample == NULL) {
    PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1);
  }

  r = prom_metric_sample_add(count_sample, 1.0);
  if (r) {
    PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1);
  }

  // Update the sum sample
  const char *sum_l_value = prom_map_get(self->l_values, "sum");
  if (sum_l_value == NULL) {
    PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1);
  }

  prom_metric_sample_t *sum_sample = prom_map_get(self->samples, sum_l_value);
  if (sum_sample == NULL) {
    PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(1);
  }

  r = prom_metric_sample_add(sum_sample, value);
  PROM_METRIC_SAMPLE_HISTOGRAM_OBSERVE_HANDLE_UNLOCK(r);
  return r;
}

static const char *prom_metric_sample_histogram_l_value_for_bucket(prom_metric_sample_histogram_t *self,
                                                                   const char *name, size_t label_count,
                                                                   const char **label_keys, const char **label_values,
                                                                   double bucket) {
  PROM_ASSERT(self != NULL);
  int r = 0;

#define PROM_METRIC_SAMPLE_HISTOGRAM_L_VALUE_FOR_BUCKET_CLEANUP() \
  for (size_t i = 0; i < label_count + 1; i++) {                  \
    prom_free((char *)new_keys[i]);                               \
    prom_free((char *)new_values[i]);                             \
  }                                                               \
  prom_free(new_keys);                                            \
  prom_free(new_values);

  // Make new array to hold label_keys with le label key
  const char **new_keys = (const char **)prom_malloc((label_count + 1) * sizeof(char *));
  for (size_t i = 0; i < label_count; i++) {
    new_keys[i] = prom_strdup(label_keys[i]);
  }
  new_keys[label_count] = prom_strdup("le");

  // Make new array to hold label_values with le label value
  const char **new_values = (const char **)prom_malloc((label_count + 1) * sizeof(char *));
  for (size_t i = 0; i < label_count; i++) {
    new_values[i] = prom_strdup(label_values[i]);
  }

  new_values[label_count] = prom_metric_sample_histogram_bucket_to_str(bucket);

  r = prom_metric_formatter_load_l_value(self->metric_formatter, name, NULL, label_count + 1, new_keys, new_values);
  if (r) {
    PROM_METRIC_SAMPLE_HISTOGRAM_L_VALUE_FOR_BUCKET_CLEANUP();
    return NULL;
  }
  const char *ret = (const char *)prom_metric_formatter_dump(self->metric_formatter);
  PROM_METRIC_SAMPLE_HISTOGRAM_L_VALUE_FOR_BUCKET_CLEANUP();
  return ret;
}

static const char *prom_metric_sample_histogram_l_value_for_inf(prom_metric_sample_histogram_t *self, const char *name,
                                                                size_t label_count, const char **label_keys,
                                                                const char **label_values) {
  PROM_ASSERT(self != NULL);
  int r = 0;

#define PROM_METRIC_SAMPLE_HISTOGRAM_L_VALUE_FOR_INF_CLEANUP() \
  for (size_t i = 0; i < label_count + 1; i++) {               \
    prom_free((char *)new_keys[i]);                            \
    prom_free((char *)new_values[i]);                          \
  }                                                            \
  prom_free(new_keys);                                         \
  prom_free(new_values);

  // Make new array to hold label_keys with le label key
  const char **new_keys = (const char **)prom_malloc((label_count + 1) * sizeof(char *));
  for (size_t i = 0; i < label_count; i++) {
    new_keys[i] = prom_strdup(label_keys[i]);
  }
  new_keys[label_count] = prom_strdup("le");

  // Make new array to hold label_values with le label value
  const char **new_values = (const char **)prom_malloc((label_count + 1) * sizeof(char *));
  for (size_t i = 0; i < label_count; i++) {
    new_values[i] = prom_strdup(label_values[i]);
  }

  new_values[label_count] = prom_strdup("+Inf");

  r = prom_metric_formatter_load_l_value(self->metric_formatter, name, NULL, label_count + 1, new_keys, new_values);
  if (r) {
    PROM_METRIC_SAMPLE_HISTOGRAM_L_VALUE_FOR_INF_CLEANUP()
    return NULL;
  }
  const char *ret = (const char *)prom_metric_formatter_dump(self->metric_formatter);
  PROM_METRIC_SAMPLE_HISTOGRAM_L_VALUE_FOR_INF_CLEANUP()
  return ret;
}

static void prom_metric_sample_histogram_free_str_generic(void *gen) {
  char *str = (char *)gen;
  prom_free((void *)str);
  str = NULL;
}

char *prom_metric_sample_histogram_bucket_to_str(double bucket) {
  char *buf = (char *)prom_malloc(sizeof(char) * 50);
  sprintf(buf, "%g", bucket);
  if (!strchr(buf, '.')) {
    strcat(buf, ".0");
  }
  return buf;
}

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

// Public
#include "prom_counter.h"

#include "prom_alloc.h"

// Private
#include "prom_assert.h"
#include "prom_errors.h"
#include "prom_log.h"
#include "prom_metric_i.h"
#include "prom_metric_sample_i.h"
#include "prom_metric_sample_t.h"
#include "prom_metric_t.h"

prom_counter_t *prom_counter_new(const char *name, const char *help, size_t label_key_count, const char **label_keys) {
  return (prom_counter_t *)prom_metric_new(PROM_COUNTER, name, help, label_key_count, label_keys);
}

int prom_counter_destroy(prom_counter_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 0;
  int r = 0;
  r = prom_metric_destroy(self);
  self = NULL;
  return r;
}

int prom_counter_inc(prom_counter_t *self, const char **label_values) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  if (self->type != PROM_COUNTER) {
    PROM_LOG(PROM_METRIC_INCORRECT_TYPE);
    return 1;
  }
  prom_metric_sample_t *sample = prom_metric_sample_from_labels(self, label_values);
  if (sample == NULL) return 1;
  return prom_metric_sample_add(sample, 1.0);
}

int prom_counter_add(prom_counter_t *self, double r_value, const char **label_values) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  if (self->type != PROM_COUNTER) {
    PROM_LOG(PROM_METRIC_INCORRECT_TYPE);
    return 1;
  }
  prom_metric_sample_t *sample = prom_metric_sample_from_labels(self, label_values);
  if (sample == NULL) return 1;
  return prom_metric_sample_add(sample, r_value);
}

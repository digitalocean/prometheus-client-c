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

#include <stdatomic.h>

// Public
#include "prom_alloc.h"

// Private
#include "prom_assert.h"
#include "prom_errors.h"
#include "prom_log.h"
#include "prom_metric_sample_i.h"
#include "prom_metric_sample_t.h"

prom_metric_sample_t *prom_metric_sample_new(prom_metric_type_t type, const char *l_value, double r_value) {
  prom_metric_sample_t *self = (prom_metric_sample_t *)prom_malloc(sizeof(prom_metric_sample_t));
  self->type = type;
  self->l_value = prom_strdup(l_value);
  self->r_value = ATOMIC_VAR_INIT(r_value);
  return self;
}

int prom_metric_sample_destroy(prom_metric_sample_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 0;
  prom_free((void *)self->l_value);
  self->l_value = NULL;
  prom_free((void *)self);
  self = NULL;
  return 0;
}

int prom_metric_sample_destroy_generic(void *gen) {
  int r = 0;

  prom_metric_sample_t *self = (prom_metric_sample_t *)gen;
  r = prom_metric_sample_destroy(self);
  self = NULL;
  return r;
}

void prom_metric_sample_free_generic(void *gen) {
  prom_metric_sample_t *self = (prom_metric_sample_t *)gen;
  prom_metric_sample_destroy(self);
}

int prom_metric_sample_add(prom_metric_sample_t *self, double r_value) {
  PROM_ASSERT(self != NULL);
  if (r_value < 0) {
    return 1;
  }
  _Atomic double old = atomic_load(&self->r_value);
  for (;;) {
    _Atomic double new = ATOMIC_VAR_INIT(old + r_value);
    if (atomic_compare_exchange_weak(&self->r_value, &old, new)) {
      return 0;
    }
  }
}

int prom_metric_sample_sub(prom_metric_sample_t *self, double r_value) {
  PROM_ASSERT(self != NULL);
  if (self->type != PROM_GAUGE) {
    PROM_LOG(PROM_METRIC_INCORRECT_TYPE);
    return 1;
  }
  _Atomic double old = atomic_load(&self->r_value);
  for (;;) {
    _Atomic double new = ATOMIC_VAR_INIT(old - r_value);
    if (atomic_compare_exchange_weak(&self->r_value, &old, new)) {
      return 0;
    }
  }
}

int prom_metric_sample_set(prom_metric_sample_t *self, double r_value) {
  if (self->type != PROM_GAUGE) {
    PROM_LOG(PROM_METRIC_INCORRECT_TYPE);
    return 1;
  }
  atomic_store(&self->r_value, r_value);
  return 0;
}

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

#include <stdarg.h>
#include <stdlib.h>

// Public
#include "prom_alloc.h"
#include "prom_histogram_buckets.h"

// Private
#include "prom_assert.h"
#include "prom_log.h"

prom_histogram_buckets_t *prom_histogram_default_buckets = NULL;

prom_histogram_buckets_t *prom_histogram_buckets_new(size_t count, double bucket, ...) {
  prom_histogram_buckets_t *self = (prom_histogram_buckets_t *)prom_malloc(sizeof(prom_histogram_buckets_t));
  self->count = count;
  double *upper_bounds = (double *)prom_malloc(sizeof(double) * count);
  upper_bounds[0] = bucket;
  if (count == 1) {
    self->upper_bounds = upper_bounds;
    return self;
  }
  va_list arg_list;
  va_start(arg_list, bucket);
  for (int i = 1; i < count; i++) {
    upper_bounds[i] = va_arg(arg_list, double);
  }
  va_end(arg_list);
  self->upper_bounds = upper_bounds;
  return self;
}

prom_histogram_buckets_t *prom_histogram_buckets_linear(double start, double width, size_t count) {
  if (count <= 1) return NULL;

  prom_histogram_buckets_t *self = (prom_histogram_buckets_t *)prom_malloc(sizeof(prom_histogram_buckets_t));

  double *upper_bounds = (double *)prom_malloc(sizeof(double) * count);
  upper_bounds[0] = start;
  for (size_t i = 1; i < count; i++) {
    upper_bounds[i] = upper_bounds[i - 1] + width;
  }
  self->upper_bounds = upper_bounds;
  self->count = count;
  return self;
}

prom_histogram_buckets_t *prom_histogram_buckets_exponential(double start, double factor, size_t count) {
  if (count < 1) {
    PROM_LOG("count must be less than 1");
    return NULL;
  }
  if (start <= 0) {
    PROM_LOG("start must be less than or equal to 0");
    return NULL;
  }
  if (factor <= 1) {
    PROM_LOG("factor must be less than or equal to 1");
    return NULL;
  }

  prom_histogram_buckets_t *self = (prom_histogram_buckets_t *)prom_malloc(sizeof(prom_histogram_buckets_t));

  double *upper_bounds = (double *)prom_malloc(sizeof(double) * count);
  upper_bounds[0] = start;
  for (size_t i = 1; i < count; i++) {
    upper_bounds[i] = upper_bounds[i - 1] * factor;
  }
  self->upper_bounds = upper_bounds;
  self->count = count;
  return self;
}

int prom_histogram_buckets_destroy(prom_histogram_buckets_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 0;
  prom_free((double *)self->upper_bounds);
  self->upper_bounds = NULL;
  prom_free(self);
  self = NULL;
  return 0;
}

size_t prom_histogram_buckets_count(prom_histogram_buckets_t *self) {
  PROM_ASSERT(self != NULL);
  return self->count;
}

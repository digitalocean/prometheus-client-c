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

// Public
#include "prom_alloc.h"

// Private
#include "prom_assert.h"
#include "prom_collector_t.h"
#include "prom_linked_list_t.h"
#include "prom_map_i.h"
#include "prom_metric_formatter_i.h"
#include "prom_metric_sample_histogram_t.h"
#include "prom_metric_sample_t.h"
#include "prom_metric_t.h"
#include "prom_string_builder_i.h"

prom_metric_formatter_t *prom_metric_formatter_new() {
  prom_metric_formatter_t *self = (prom_metric_formatter_t *)prom_malloc(sizeof(prom_metric_formatter_t));
  self->string_builder = prom_string_builder_new();
  if (self->string_builder == NULL) {
    prom_metric_formatter_destroy(self);
    return NULL;
  }
  self->err_builder = prom_string_builder_new();
  if (self->err_builder == NULL) {
    prom_metric_formatter_destroy(self);
    return NULL;
  }
  return self;
}

int prom_metric_formatter_destroy(prom_metric_formatter_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 0;

  int r = 0;
  int ret = 0;

  r = prom_string_builder_destroy(self->string_builder);
  self->string_builder = NULL;
  if (r) ret = r;

  r = prom_string_builder_destroy(self->err_builder);
  self->err_builder = NULL;
  if (r) ret = r;

  prom_free(self);
  self = NULL;
  return ret;
}

int prom_metric_formatter_load_help(prom_metric_formatter_t *self, const char *name, const char *help) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;

  int r = 0;

  r = prom_string_builder_add_str(self->string_builder, "# HELP ");
  if (r) return r;

  r = prom_string_builder_add_str(self->string_builder, name);
  if (r) return r;

  r = prom_string_builder_add_char(self->string_builder, ' ');
  if (r) return r;

  r = prom_string_builder_add_str(self->string_builder, help);
  if (r) return r;

  return prom_string_builder_add_char(self->string_builder, '\n');
}

int prom_metric_formatter_load_type(prom_metric_formatter_t *self, const char *name, prom_metric_type_t metric_type) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;

  int r = 0;

  r = prom_string_builder_add_str(self->string_builder, "# TYPE ");
  if (r) return r;

  r = prom_string_builder_add_str(self->string_builder, name);
  if (r) return r;

  r = prom_string_builder_add_char(self->string_builder, ' ');
  if (r) return r;

  r = prom_string_builder_add_str(self->string_builder, prom_metric_type_map[metric_type]);
  if (r) return r;

  return prom_string_builder_add_char(self->string_builder, '\n');
}

int prom_metric_formatter_load_l_value(prom_metric_formatter_t *self, const char *name, const char *suffix,
                                       size_t label_count, const char **label_keys, const char **label_values) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;

  int r = 0;

  r = prom_string_builder_add_str(self->string_builder, name);
  if (r) return r;

  if (suffix != NULL) {
    r = prom_string_builder_add_char(self->string_builder, '_');
    if (r) return r;

    r = prom_string_builder_add_str(self->string_builder, suffix);
    if (r) return r;
  }

  if (label_count == 0) return 0;

  for (int i = 0; i < label_count; i++) {
    if (i == 0) {
      r = prom_string_builder_add_char(self->string_builder, '{');
      if (r) return r;
    }
    r = prom_string_builder_add_str(self->string_builder, (const char *)label_keys[i]);
    if (r) return r;

    r = prom_string_builder_add_char(self->string_builder, '=');
    if (r) return r;

    r = prom_string_builder_add_char(self->string_builder, '"');
    if (r) return r;

    r = prom_string_builder_add_str(self->string_builder, (const char *)label_values[i]);
    if (r) return r;

    r = prom_string_builder_add_char(self->string_builder, '"');
    if (r) return r;

    if (i == label_count - 1) {
      r = prom_string_builder_add_char(self->string_builder, '}');
      if (r) return r;
    } else {
      r = prom_string_builder_add_char(self->string_builder, ',');
      if (r) return r;
    }
  }
  return 0;
}

int prom_metric_formatter_load_sample(prom_metric_formatter_t *self, prom_metric_sample_t *sample) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;

  int r = 0;

  r = prom_string_builder_add_str(self->string_builder, sample->l_value);
  if (r) return r;

  r = prom_string_builder_add_char(self->string_builder, ' ');
  if (r) return r;

  char buffer[50];
  sprintf(buffer, "%.17g", sample->r_value);
  r = prom_string_builder_add_str(self->string_builder, buffer);
  if (r) return r;

  return prom_string_builder_add_char(self->string_builder, '\n');
}

int prom_metric_formatter_clear(prom_metric_formatter_t *self) {
  PROM_ASSERT(self != NULL);
  return prom_string_builder_clear(self->string_builder);
}

char *prom_metric_formatter_dump(prom_metric_formatter_t *self) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  if (self == NULL) return NULL;
  char *data = prom_string_builder_dump(self->string_builder);
  if (data == NULL) return NULL;
  r = prom_string_builder_clear(self->string_builder);
  if (r) {
    prom_free(data);
    return NULL;
  }
  return data;
}

int prom_metric_formatter_load_metric(prom_metric_formatter_t *self, prom_metric_t *metric) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;

  int r = 0;

  r = prom_metric_formatter_load_help(self, metric->name, metric->help);
  if (r) return r;

  r = prom_metric_formatter_load_type(self, metric->name, metric->type);
  if (r) return r;

  for (prom_linked_list_node_t *current_node = metric->samples->keys->head; current_node != NULL;
       current_node = current_node->next) {
    const char *key = (const char *)current_node->item;
    if (metric->type == PROM_HISTOGRAM) {
      prom_metric_sample_histogram_t *hist_sample =
          (prom_metric_sample_histogram_t *)prom_map_get(metric->samples, key);

      if (hist_sample == NULL) return 1;

      for (prom_linked_list_node_t *current_hist_node = hist_sample->l_value_list->head; current_hist_node != NULL;
           current_hist_node = current_hist_node->next) {
        const char *hist_key = (const char *)current_hist_node->item;
        prom_metric_sample_t *sample = (prom_metric_sample_t *)prom_map_get(hist_sample->samples, hist_key);
        if (sample == NULL) return 1;
        r = prom_metric_formatter_load_sample(self, sample);
        if (r) return r;
      }
    } else {
      prom_metric_sample_t *sample = (prom_metric_sample_t *)prom_map_get(metric->samples, key);
      if (sample == NULL) return 1;
      r = prom_metric_formatter_load_sample(self, sample);
      if (r) return r;
    }
  }
  return prom_string_builder_add_char(self->string_builder, '\n');
}

int prom_metric_formatter_load_metrics(prom_metric_formatter_t *self, prom_map_t *collectors) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  for (prom_linked_list_node_t *current_node = collectors->keys->head; current_node != NULL;
       current_node = current_node->next) {
    const char *collector_name = (const char *)current_node->item;
    prom_collector_t *collector = (prom_collector_t *)prom_map_get(collectors, collector_name);
    if (collector == NULL) return 1;

    prom_map_t *metrics = collector->collect_fn(collector);
    if (metrics == NULL) return 1;

    for (prom_linked_list_node_t *current_node = metrics->keys->head; current_node != NULL;
         current_node = current_node->next) {
      const char *metric_name = (const char *)current_node->item;
      prom_metric_t *metric = (prom_metric_t *)prom_map_get(metrics, metric_name);
      if (metric == NULL) return 1;
      r = prom_metric_formatter_load_metric(self, metric);
      if (r) return r;
    }
  }
  return r;
}

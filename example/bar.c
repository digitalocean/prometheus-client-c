/**
 * Copyright 2019 DigitalOcean Inc.
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

#include "prom.h"

prom_counter_t *bar_counter;
prom_gauge_t *bar_gauge;

int bar(int v, const char *label) {
  int r = 0;
  r = prom_counter_inc(bar_counter, NULL);
  if (r) return r;
  return prom_gauge_add(bar_gauge, v, (const char *[]) { label });
}

void bar_init(void) {
  bar_counter = prom_collector_registry_must_register_metric(prom_counter_new("bar_counter", "counter for bar", 0, NULL));
  bar_gauge = prom_collector_registry_must_register_metric(prom_gauge_new("bar_gauge", "gauge for bar", 1, (const char *[]) { "label" }));
}
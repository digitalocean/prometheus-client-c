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

#ifndef PROM_METRIC_FORMATTER_T_H
#define PROM_METRIC_FORMATTER_T_H

#include "prom_string_builder_t.h"

typedef struct prom_metric_formatter {
  prom_string_builder_t *string_builder;
  prom_string_builder_t *err_builder;
} prom_metric_formatter_t;

#endif  // PROM_METRIC_FORMATTER_T_H

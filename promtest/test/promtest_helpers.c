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

#include "promtest_helpers.h"

#include <stdio.h>
#include <stdlib.h>

#include "prom.h"

struct MHD_Daemon *promtest_daemon;

promtest_popen_buf_t *promtest_popen_buf_new(FILE *f) {
  promtest_popen_buf_t *self = (promtest_popen_buf_t *)malloc(sizeof(promtest_popen_buf_t));
  self->buf = malloc(32);
  self->size = 32;
  self->allocated = 32;
  self->f = f;
  return self;
}

int promtest_popen_buf_ensure_space(promtest_popen_buf_t *self) {
  if (self->allocated >= self->size + 1) return 0;
  while (self->allocated < self->size + 1) self->allocated <<= 1;
  self->buf = (char *)prom_realloc(self->buf, self->allocated);
  return 0;
}

int promtest_popen_buf_destroy(promtest_popen_buf_t *self) {
  if (self == NULL) {
    return 0;
  }
  free((void *)self->buf);
  self->buf = NULL;
  free((void *)self);
  self = NULL;
  return 0;
}

int promtest_popen_buf_read(promtest_popen_buf_t *self) {
  for (int current_char = fgetc(self->f), i = 0; current_char != EOF; current_char = fgetc(self->f), i++) {
    promtest_popen_buf_ensure_space(self);
    self->buf[i] = current_char;
    self->size++;
  }
  promtest_popen_buf_ensure_space(self);
  self->buf[self->size] = '\0';
  self->size++;
  return 0;
}

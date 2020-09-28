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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// Public
#include "prom_alloc.h"

// Private
#include "prom_assert.h"
#include "prom_log.h"
#include "prom_procfs_i.h"

static int prom_procfs_ensure_buf_size(prom_procfs_buf_t *self) {
  PROM_ASSERT(self != NULL);
  if (self->allocated >= self->size + 1) return 0;
  while (self->allocated < self->size + 1) self->allocated <<= 1;
  self->buf = (char *)prom_realloc(self->buf, self->allocated);
  return 0;
}

prom_procfs_buf_t *prom_procfs_buf_new(const char *path) {
  int r = 0;

  FILE *f = fopen(path, "r");
  char errbuf[100];

  if (f == NULL) {
    strerror_r(errno, errbuf, 100);
    PROM_LOG(errbuf);
    return NULL;
  }

#define PROM_PROCFS_BUF_NEW_HANDLE_F_CLOSE(f) \
  r = fclose(f);                              \
  if (r) {                                    \
    strerror_r(errno, errbuf, 100);           \
    PROM_LOG(errbuf);                         \
  }

  unsigned short int initial_size = 32;
  prom_procfs_buf_t *self = prom_malloc(sizeof(prom_procfs_buf_t));
  self->buf = prom_malloc(initial_size);
  self->size = 0;
  self->index = 0;
  self->allocated = initial_size;

  for (int current_char = getc(f), i = 0; current_char != EOF; current_char = getc(f), i++) {
    r = prom_procfs_ensure_buf_size(self);
    if (r) {
      prom_procfs_buf_destroy(self);
      self = NULL;
      PROM_PROCFS_BUF_NEW_HANDLE_F_CLOSE(f);
      return NULL;
    }
    self->buf[i] = current_char;
    self->size++;
  }
  r = prom_procfs_ensure_buf_size(self);
  if (r) {
    prom_procfs_buf_destroy(self);
    self = NULL;
    PROM_PROCFS_BUF_NEW_HANDLE_F_CLOSE(f);
    return NULL;
  }

  self->buf[self->size] = '\0';
  self->size++;

  PROM_PROCFS_BUF_NEW_HANDLE_F_CLOSE(f);
  if (r) return NULL;
  return self;
}

int prom_procfs_buf_destroy(prom_procfs_buf_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 0;
  prom_free(self->buf);
  prom_free(self);
  self = NULL;
  return 0;
}

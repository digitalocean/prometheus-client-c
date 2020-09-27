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

#ifndef PROMTTEST_HELPERS_H
#define PROMTTEST_HELPERS_H

#define PROMTEST_THREAD_POOL_SIZE 5

extern struct MHD_Daemon *promtest_daemon;

typedef struct promtest_popen_buf {
  char *buf;
  int size;
  int allocated;
  FILE *f;
} promtest_popen_buf_t;

promtest_popen_buf_t *promtest_popen_buf_new(FILE *f);
int promtest_popen_buf_destroy(promtest_popen_buf_t *self);
int promtest_popen_buf_read(promtest_popen_buf_t *self);
int promtest_popen_buf_ensure_space(promtest_popen_buf_t *self);

#endif  // PROMTEST_HELPERS_H

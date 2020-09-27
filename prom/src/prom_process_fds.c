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

#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

// Public
#include "prom_alloc.h"
#include "prom_gauge.h"

// Private
#include "prom_errors.h"
#include "prom_log.h"
#include "prom_process_fds_t.h"

prom_gauge_t *prom_process_open_fds;

int prom_process_fds_count(const char *path) {
  int count = 0;
  int r = 0;
  struct dirent *de;
  DIR *d;
  if (path) {
    d = opendir(path);
    if (d == NULL) {
      PROM_LOG(PROM_STDIO_OPEN_DIR_ERROR);
      return -1;
    }
  } else {
    int pid = (int)getpid();
    char p[50];
    sprintf(p, "/proc/%d/fd", pid);
    d = opendir(p);
    if (d == NULL) {
      PROM_LOG(PROM_STDIO_OPEN_DIR_ERROR);
      return -1;
    }
  }

  while ((de = readdir(d)) != NULL) {
    if (strcmp(".", de->d_name) == 0 || strcmp("..", de->d_name) == 0) {
      continue;
    }
    count++;
  }
  r = closedir(d);
  if (r) {
    PROM_LOG(PROM_STDIO_CLOSE_DIR_ERROR);
    return -1;
  }
  return count;
}

int prom_process_fds_init(void) {
  prom_process_open_fds = prom_gauge_new("process_open_fds", "Number of open file descriptors.", 0, NULL);
  return 0;
}

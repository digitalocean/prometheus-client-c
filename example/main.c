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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "microhttpd.h"
#include "prom.h"
#include "promhttp.h"
#include "foo.h"
#include "bar.h"


prom_histogram_t *test_histogram;

static void init(void) {
  // Initialize the Default registry
  prom_collector_registry_default_init();

  // Register file-based metrics for each file
  foo_init();
  bar_init();

  test_histogram = prom_collector_registry_must_register_metric(
    prom_histogram_new(
      "test_histogram",
      "histogram under test",
      prom_histogram_buckets_linear(5.0, 5.0, 2),
      0,
      NULL
    )
  );

  // Set the active registry for the HTTP handler
  promhttp_set_active_collector_registry(NULL);
}

int main(int argc, const char **argv) {
  init();
  int r = 0;
  const char *labels[] = { "one", "two", "three", "four", "five" };
  for (int i = 1; i <= 100; i++) {
    double hist_value;
    if (i % 2 == 0) {
      hist_value = 3.0;
    } else {
      hist_value = 7.0;
    }

    r = prom_histogram_observe(test_histogram, hist_value, NULL);
    if (r) exit(1);

    for (int x = 0; x < 5; x++) {
      r = foo(i,  labels[x]);
      if (r) exit(r);
      r = bar(i+x, labels[x]);
      if (r) exit(r);
    }
  }


  struct MHD_Daemon *daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
  if (daemon == NULL) {
    return 1;
  }

  int done = 0;

  auto void intHandler(int signal);
  void intHandler(int signal) {
    printf("\nshutting down...\n");
    fflush(stdout);
    prom_collector_registry_destroy(PROM_COLLECTOR_REGISTRY_DEFAULT);
    MHD_stop_daemon(daemon);
    done = 1;
  }

  if (argc == 2) {
    unsigned int timeout = atoi(argv[1]);
    sleep(timeout);
    intHandler(0);
    return 0;
  }

  signal(SIGINT, intHandler);
  while(done == 0) {}

  return 0;
}
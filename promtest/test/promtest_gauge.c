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

#include "promtest_gauge.h"

#include <pthread.h>

#include "parson.h"
#include "prom.h"
#include "promhttp.h"
#include "promtest_helpers.h"
#include "unity.h"

static void *promtest_gauge_handler(void *data);
static int promtest_parse_gauge_output(const char *output, char **value);
prom_gauge_t *foo_gauge;

/**
 * @brief For each thread in a threadpool of 10 we increment a single gauge 1
 * million times
 *
 * The purpose of this test is to check for deadlock and race conditions
 */
void promtest_gauge(void) {
  if (promtest_gauge_setup()) {
    TEST_FAIL_MESSAGE("failed to setup promtest_gauge");
  }

  void *retvals[PROMTEST_THREAD_POOL_SIZE];
  pthread_t thread_pool[PROMTEST_THREAD_POOL_SIZE];

  // Start each thread
  for (int i = 0; i < PROMTEST_THREAD_POOL_SIZE; i++) {
    if (pthread_create(&(thread_pool[i]), NULL, promtest_gauge_handler, NULL)) {
      TEST_FAIL_MESSAGE("failed to create thread");
    }
  }

  // Join each thread
  for (int i = 0; i < PROMTEST_THREAD_POOL_SIZE; i++) {
    if (pthread_join(thread_pool[i], (void **)&(retvals[i]))) {
      TEST_FAIL_MESSAGE("thread failed to join");
    }
  }

  // verify clean exit for each thread
  for (int i = 0; i < PROMTEST_THREAD_POOL_SIZE; i++) {
    if (*((int *)retvals[i]) != 0) {
      TEST_FAIL_MESSAGE("thread did not exit properly");
    }
  }

  // scrape the endpoint
  FILE *f = popen("prom2json http://0.0.0.0:8000/metrics", "r");
  if (f == NULL) {
    TEST_FAIL_MESSAGE("shell out failed");
  }
  promtest_popen_buf_t *buf = promtest_popen_buf_new(f);
  if (promtest_popen_buf_read(buf)) {
    TEST_FAIL_MESSAGE("failed to scrape endpoint");
  }

  const char *output = strdup(buf->buf);
  if (promtest_popen_buf_destroy(buf)) {
    TEST_FAIL_MESSAGE("failed to duplicate buf");
  }

  // Parse the output
  char *value = (char *)malloc(sizeof(char) * 100);
  if (promtest_parse_gauge_output(output, &value)) {
    TEST_FAIL_MESSAGE("failed to parse output");
  }

  // Assert
  TEST_ASSERT_EQUAL_STRING("5e+06", value);
  if (promtest_gauge_teardown()) {
    TEST_FAIL_MESSAGE("failed to teardown promtest_gauge");
  }

  free(value);
}

int promtest_gauge_setup(void) {
  // Initialize the default collector registry
  prom_collector_registry_default_init();

  // Set the gauge
  foo_gauge = prom_collector_registry_must_register_metric(prom_gauge_new("foo_gauge", "gauge for foo", 0, NULL));

  // Set the collector registry on the handler to the default registry
  promhttp_set_active_collector_registry(NULL);

  // Start the HTTP server
  promtest_daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);

  if (promtest_daemon == NULL)
    return 1;
  else
    return 0;
}

int promtest_gauge_teardown(void) {
  // Destroy the default registry. This effectively deallocates all metrics
  // registered to it, including itself
  prom_collector_registry_destroy(PROM_COLLECTOR_REGISTRY_DEFAULT);

  PROM_COLLECTOR_REGISTRY_DEFAULT = NULL;

  // Stop the HTTP server
  MHD_stop_daemon(promtest_daemon);

  return 0;
}

/**
 * @brief The entrypoint to a worker thread within the prom_gauge_test
 */
static void *promtest_gauge_handler(void *data) {
  for (int i = 0; i < 1000000; i++) {
    prom_gauge_inc(foo_gauge, NULL);
  }
  int *retval = (int *)malloc(sizeof(int));
  *retval = 0;
  return (void *)retval;
}

/**
 * @brief Parse the output and set the value of the foo_gauge metric.
 *
 * We must past a pointer to a char* so the value gets updated
 */
static int promtest_parse_gauge_output(const char *output, char **value) {
  // Parse the JSON output
  JSON_Value *root = json_parse_string(output);
  if (json_value_get_type(root) != JSONArray) {
    TEST_FAIL_MESSAGE("JSON Parse error...expected JSONArray type");
  }

  JSON_Array *collection = json_value_get_array(root);
  if (collection == NULL) {
    TEST_FAIL_MESSAGE("Failed to extract array from JSON_Value");
  }

  for (int i = 0; i < json_array_get_count(collection); i++) {
    JSON_Object *obj = json_array_get_object(collection, i);
    if (obj == NULL) {
      TEST_FAIL_MESSAGE("failed to retrieve single object within JSON_Array");
    }
    const char *name = json_object_get_string(obj, "name");
    if (strcmp(name, "foo_gauge")) {
      continue;
    }
    JSON_Array *samples = json_object_dotget_array(obj, "metrics");
    if (samples == NULL) {
      TEST_FAIL_MESSAGE("failed to retrieve metrics from JSON_Object");
    }
    if (json_array_get_count(samples) < 1) {
      TEST_FAIL_MESSAGE("No samples found");
    }
    JSON_Object *sample = json_array_get_object(samples, 0);
    if (sample == NULL) {
      TEST_FAIL_MESSAGE("failed to get metric sample");
    }
    *value = (char *)json_object_get_string(sample, "value");
  }
  if (strlen(*value) == 0) {
    return 1;
  }
  return 0;
}

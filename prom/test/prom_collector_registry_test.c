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

#include "prom_test_helpers.h"

static void prom_registry_test_init(void);
static void prom_registry_test_destroy(void);

prom_counter_t *test_counter;
prom_gauge_t *test_gauge;
prom_histogram_t *test_histogram;

void test_large_registry(void) {
  prom_collector_registry_default_init();
  for (int i = 0; i < 1000; i++) {
    char metric[6];
    sprintf(metric, "%d", i);
    prom_collector_registry_must_register_metric(prom_counter_new(metric, metric, 0, NULL));
  }
  prom_registry_test_destroy();
}

void test_prom_collector_registry_must_register(void) {
  prom_registry_test_init();
  int r = 0;

  const char *labels[] = {"foo"};
  r = prom_counter_inc(test_counter, labels);
  if (r) TEST_FAIL();
  r = prom_gauge_add(test_gauge, 2.0, labels);
  if (r) TEST_FAIL();

  prom_metric_sample_t *test_sample_a = prom_metric_sample_from_labels(test_counter, labels);
  prom_metric_sample_t *test_sample_b = prom_metric_sample_from_labels(test_gauge, labels);

  TEST_ASSERT_EQUAL_DOUBLE(1.0, test_sample_a->r_value);
  TEST_ASSERT_EQUAL_DOUBLE(2.0, test_sample_b->r_value);
  prom_registry_test_destroy();
}

void test_prom_collector_registry_bridge(void) {
  prom_registry_test_init();
  int r = 0;

  if (test_histogram == NULL) TEST_FAIL_MESSAGE("histogram failed to initialize");

  const char *labels[] = {"foo"};
  prom_counter_inc(test_counter, labels);
  prom_gauge_set(test_gauge, 2.0, labels);
  r = prom_histogram_observe(test_histogram, 3.0, NULL);
  if (r) TEST_FAIL();
  r = prom_histogram_observe(test_histogram, 7.0, NULL);
  if (r) TEST_FAIL();

  const char *result = prom_collector_registry_bridge(PROM_COLLECTOR_REGISTRY_DEFAULT);

  const char *expected[] = {
      "# HELP test_counter counter under test",
      "# TYPE test_counter counter",
      "test_counter{label=\"foo\"}",
      "HELP test_gauge gauge under test",
      "# TYPE test_gauge gauge",
      "test_gauge{label=\"foo\"}",
      "# HELP test_histogram histogram under test",
      "# TYPE test_histogram histogram\ntest_histogram{le=\"5.0\"}",
      "test_histogram{le=\"10.0\"}",
      "test_histogram{le=\"+Inf\"}",
      "test_histogram_count",
      "test_histogram_sum",
      "# HELP process_max_fds Maximum number of open file descriptors.",
      "# TYPE process_max_fds gauge",
      "process_max_fds",
      "# HELP process_virtual_memory_max_bytes Maximum amount of virtual memory available in bytes.",
      "# TYPE process_virtual_memory_max_bytes"};

  for (int i = 0; i < 17; i++) {
    TEST_ASSERT_NOT_NULL(strstr(result, expected[i]));
  }

  free((char *)result);
  result = NULL;

  prom_registry_test_destroy();
}

void test_prom_collector_registry_validate_metric_name(void) {
  prom_registry_test_init();

  TEST_ASSERT_EQUAL_INT(
      0, prom_collector_registry_validate_metric_name(PROM_COLLECTOR_REGISTRY_DEFAULT, "this_is_a_name09"));
  prom_registry_test_destroy();
}

void prom_registry_test_init(void) {
  prom_collector_registry_default_init();
  const char *label[] = {"label"};
  test_counter =
      prom_collector_registry_must_register_metric(prom_counter_new("test_counter", "counter under test", 1, label));
  test_gauge = prom_collector_registry_must_register_metric(prom_gauge_new("test_gauge", "gauge under test", 1, label));
  test_histogram = prom_collector_registry_must_register_metric(prom_histogram_new(
      "test_histogram", "histogram under test", prom_histogram_buckets_linear(5.0, 5.0, 2), 0, NULL));
}

static void prom_registry_test_destroy(void) {
  int r = 0;
  r = prom_collector_registry_destroy(PROM_COLLECTOR_REGISTRY_DEFAULT);
  PROM_COLLECTOR_REGISTRY_DEFAULT = NULL;
  if (r) TEST_FAIL_MESSAGE("failed to destroy PROM_COLLECTOR_REGISTRY_DEFAULT");
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  // RUN_TEST(test_prom_collector_registry_must_register);
  RUN_TEST(test_prom_collector_registry_bridge);
  // RUN_TEST(test_prom_collector_registry_validate_metric_name);
  // RUN_TEST(test_large_registry);
  return UNITY_END();
}

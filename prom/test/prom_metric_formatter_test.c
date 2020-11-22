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

void test_prom_metric_formatter_load_l_value(void) {
  prom_metric_formatter_t *mf = prom_metric_formatter_new();
  const char *keys[] = {"foo", "bar", "bing"};
  const char *values[] = {"one", "two", "three"};
  prom_metric_formatter_load_l_value(mf, "test", NULL, 3, keys, values);
  char *actual = prom_metric_formatter_dump(mf);
  char *expected = "test{foo=\"one\",bar=\"two\",bing=\"three\"}";
  TEST_ASSERT_NOT_NULL(strstr(actual, expected));

  free(actual);
  actual = NULL;
  prom_metric_formatter_destroy(mf);
  mf = NULL;
}

void test_prom_metric_formatter_load_sample(void) {
  prom_metric_formatter_t *mf = prom_metric_formatter_new();
  const char *l_value = "test{foo=\"one\",bar=\"two\",bing=\"three\"}";
  prom_metric_sample_t *sample = prom_metric_sample_new(PROM_COUNTER, l_value, 22.2);
  prom_metric_formatter_load_sample(mf, sample);
  char *actual = prom_metric_formatter_dump(mf);
  char *expected = "test{foo=\"one\",bar=\"two\",bing=\"three\"}";
  TEST_ASSERT_NOT_NULL(strstr(actual, expected));

  free(actual);
  actual = NULL;
  prom_metric_sample_destroy(sample);
  prom_metric_formatter_destroy(mf);
  mf = NULL;
}

void test_prom_metric_formatter_load_metric(void) {
  prom_metric_formatter_t *mf = prom_metric_formatter_new();
  const char *counter_keys[] = {"foo", "bar"};
  const char *sample_a[] = {"f", "b"};
  const char *sample_b[] = {"o", "r"};
  prom_metric_t *m = prom_metric_new(PROM_COUNTER, "test_counter", "counter under test", 2, counter_keys);
  prom_metric_sample_t *s_a = prom_metric_sample_from_labels(m, sample_a);
  prom_metric_sample_add(s_a, 2.3);
  prom_metric_sample_t *s_b = prom_metric_sample_from_labels(m, sample_b);
  prom_metric_sample_add(s_b, 4.6);
  prom_metric_formatter_load_metric(mf, m);
  const char *result = prom_metric_formatter_dump(mf);

  char *substr =
      "# HELP test_counter counter under test\n# TYPE test_counter counter\ntest_counter{foo=\"f\",bar=\"b\"}";

  TEST_ASSERT_NOT_NULL(strstr(result, substr));

  substr = "\ntest_counter{foo=\"o\",bar=\"r\"}";
  TEST_ASSERT_NOT_NULL(strstr(result, substr));

  free((char *)result);
  result = NULL;
  prom_metric_destroy(m);
  m = NULL;
  prom_metric_formatter_destroy(mf);
  mf = NULL;
}

void test_prom_metric_formatter_load_metrics(void) {
  int r = 0;

  prom_metric_formatter_t *mf = prom_metric_formatter_new();
  const char *counter_keys[] = {};
  prom_collector_registry_default_init();
  prom_metric_t *m_a = prom_metric_new(PROM_COUNTER, "test_counter_a", "counter under test", 0, counter_keys);
  prom_metric_t *m_b = prom_metric_new(PROM_COUNTER, "test_counter_b", "counter under test", 0, counter_keys);
  prom_metric_sample_t *s_a = prom_metric_sample_from_labels(m_a, counter_keys);
  prom_metric_sample_add(s_a, 2.3);
  prom_metric_sample_t *s_b = prom_metric_sample_from_labels(m_b, counter_keys);
  prom_metric_sample_add(s_b, 4.6);
  prom_collector_registry_register_metric(m_a);
  prom_collector_registry_register_metric(m_b);

  prom_metric_formatter_load_metrics(mf, PROM_COLLECTOR_REGISTRY_DEFAULT->collectors);

  const char *result = prom_metric_formatter_dump(mf);
  const char *expected[] = {
      "# HELP test_counter_a counter under test",
      "# TYPE test_counter_a counter",
      "test_counter_a",
      "# HELP test_counter_b counter under test",
      "# TYPE test_counter_b counter",
      "test_counter_b",
      "# HELP process_max_fds Maximum number of open file descriptors.",
      "# TYPE process_max_fds gauge",
      "process_max_fds 1048576",
      "# HELP process_virtual_memory_max_bytes Maximum amount of virtual memory available in bytes.",
      "# TYPE process_virtual_memory_max_bytes gauge",
      "process_virtual_memory_max_bytes -1"};

  for (int i = 0; i < 12; i++) {
    TEST_ASSERT_NOT_NULL(strstr(result, expected[i]));
  }

  free((char *)result);
  result = NULL;

  r = prom_metric_formatter_destroy(mf);
  if (r) {
    TEST_FAIL_MESSAGE("Failed to destroy metric formatter");
  }
  mf = NULL;
  r = prom_collector_registry_destroy(PROM_COLLECTOR_REGISTRY_DEFAULT);
  if (r) {
    TEST_FAIL_MESSAGE("Failed to destroy default collector registry");
  }
  PROM_COLLECTOR_REGISTRY_DEFAULT = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_metric_formatter_load_l_value);
  RUN_TEST(test_prom_metric_formatter_load_sample);
  RUN_TEST(test_prom_metric_formatter_load_metric);
  RUN_TEST(test_prom_metric_formatter_load_metrics);
  return UNITY_END();
}

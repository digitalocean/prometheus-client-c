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

void test_metric_with_no_labels(void) {
  prom_metric_t *metric = prom_metric_new(PROM_GAUGE, "test_counter", "counter under test", 0, NULL);
  prom_metric_sample_t *sample = prom_metric_sample_from_labels(metric, NULL);
  prom_metric_sample_set(sample, 1.0);
  sample = prom_metric_sample_from_labels(metric, NULL);
  TEST_ASSERT_EQUAL_DOUBLE(1.0, (_Atomic double)sample->r_value);
  prom_metric_destroy(metric);
  metric = NULL;
}

void test_metric_sample_from_labels(void) {
  prom_metric_t *metric = prom_metric_new(PROM_GAUGE, "test_metric", "test counter", 2, (const char *[]){"foo", "bar"});
  const char *values[] = {"bing", "bang"};
  prom_metric_sample_t *sample = prom_metric_sample_from_labels(metric, values);
  prom_metric_sample_set(sample, 1.0);
  sample = prom_metric_sample_from_labels(metric, values);
  TEST_ASSERT_EQUAL_DOUBLE(1.0, (_Atomic double)sample->r_value);
  prom_metric_destroy(metric);
  metric = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_metric_with_no_labels);
  RUN_TEST(test_metric_sample_from_labels);
  return UNITY_END();
}

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

void test_prom_histogram(void) {
  prom_histogram_t *h =
      prom_histogram_new("test_histogram", "histogram under test", prom_histogram_buckets_linear(5.0, 5.0, 3), 0, NULL);

  prom_histogram_observe(h, 1.0, NULL);
  prom_histogram_observe(h, 7.0, NULL);
  prom_histogram_observe(h, 11.0, NULL);
  prom_histogram_observe(h, 22.0, NULL);

  prom_metric_sample_histogram_t *h_sample = prom_metric_sample_histogram_from_labels(h, NULL);

  // Test counter for each bucket
  char *bucket_key = prom_metric_sample_histogram_bucket_to_str(5.0);
  const char *l_value = prom_map_get(h_sample->l_values, bucket_key);
  prom_metric_sample_t *sample = (prom_metric_sample_t *)prom_map_get(h_sample->samples, l_value);
  TEST_ASSERT_EQUAL_STRING("test_histogram{le=\"5.0\"}", sample->l_value);
  TEST_ASSERT_EQUAL_DOUBLE(1.0, sample->r_value);
  free((char *)bucket_key);
  bucket_key = NULL;

  bucket_key = prom_metric_sample_histogram_bucket_to_str(10.0);
  l_value = prom_map_get(h_sample->l_values, bucket_key);
  sample = (prom_metric_sample_t *)prom_map_get(h_sample->samples, l_value);
  TEST_ASSERT_EQUAL_STRING("test_histogram{le=\"10.0\"}", sample->l_value);
  TEST_ASSERT_EQUAL_DOUBLE(2.0, sample->r_value);
  free((char *)bucket_key);
  bucket_key = NULL;

  bucket_key = prom_metric_sample_histogram_bucket_to_str(15.0);
  l_value = prom_map_get(h_sample->l_values, bucket_key);
  sample = (prom_metric_sample_t *)prom_map_get(h_sample->samples, l_value);
  TEST_ASSERT_EQUAL_STRING("test_histogram{le=\"15.0\"}", sample->l_value);
  TEST_ASSERT_EQUAL_DOUBLE(3.0, sample->r_value);
  free((char *)bucket_key);
  bucket_key = NULL;

  l_value = prom_map_get(h_sample->l_values, "+Inf");
  sample = (prom_metric_sample_t *)prom_map_get(h_sample->samples, l_value);
  TEST_ASSERT_EQUAL_STRING("test_histogram{le=\"+Inf\"}", sample->l_value);
  TEST_ASSERT_EQUAL_DOUBLE(4.0, sample->r_value);

  // Test total count. Should equal value ini +Inf
  l_value = prom_map_get(h_sample->l_values, "count");
  prom_metric_sample_t *sample_count = (prom_metric_sample_t *)prom_map_get(h_sample->samples, l_value);
  TEST_ASSERT_EQUAL_STRING("test_histogram_count", sample_count->l_value);
  TEST_ASSERT_EQUAL_DOUBLE(4.0, sample_count->r_value);

  // Test sum
  l_value = prom_map_get(h_sample->l_values, "sum");
  sample = (prom_metric_sample_t *)prom_map_get(h_sample->samples, l_value);
  TEST_ASSERT_EQUAL_STRING("test_histogram_sum", sample->l_value);
  TEST_ASSERT_EQUAL_DOUBLE(41.0, sample->r_value);

  prom_histogram_destroy(h);
  h = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_histogram);
  return UNITY_END();
}

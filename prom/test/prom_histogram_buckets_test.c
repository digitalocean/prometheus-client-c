
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

void test_prom_histogram_buckets_new(void) {
  prom_histogram_buckets_t *result = prom_histogram_buckets_new(3, 1.0, 2.0, 3.0);
  TEST_ASSERT_EQUAL_DOUBLE(1.0, result->upper_bounds[0]);
  TEST_ASSERT_EQUAL_DOUBLE(2.0, result->upper_bounds[1]);
  TEST_ASSERT_EQUAL_DOUBLE(3.0, result->upper_bounds[2]);
  TEST_ASSERT_EQUAL_INT(3, prom_histogram_buckets_count(result));
  prom_histogram_buckets_destroy(result);
  result = NULL;
}

void test_prom_histogram_buckets_linear(void) {
  prom_histogram_buckets_t *result = prom_histogram_buckets_linear(0.0, 1.5, 3);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, result->upper_bounds[0]);
  TEST_ASSERT_EQUAL_DOUBLE(1.5, result->upper_bounds[1]);
  TEST_ASSERT_EQUAL_DOUBLE(3.0, result->upper_bounds[2]);
  TEST_ASSERT_EQUAL_INT(3, prom_histogram_buckets_count(result));
  prom_histogram_buckets_destroy(result);
  result = NULL;
}

void test_prom_histogram_buckets_expontential(void) {
  prom_histogram_buckets_t *result = prom_histogram_buckets_exponential(1, 2, 3);
  TEST_ASSERT_EQUAL_DOUBLE(1.0, result->upper_bounds[0]);
  TEST_ASSERT_EQUAL_DOUBLE(2.0, result->upper_bounds[1]);
  TEST_ASSERT_EQUAL_DOUBLE(4.0, result->upper_bounds[2]);
  TEST_ASSERT_EQUAL_INT(3, prom_histogram_buckets_count(result));
  prom_histogram_buckets_destroy(result);
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_histogram_buckets_new);
  RUN_TEST(test_prom_histogram_buckets_linear);
  RUN_TEST(test_prom_histogram_buckets_expontential);
  return UNITY_END();
}

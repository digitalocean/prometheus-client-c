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

#include <assert.h>

#include "prom_test_helpers.h"

const char *sample_labels_a[] = {"f", "b"};
const char *sample_labels_b[] = {"o", "r"};

void test_counter_inc(void) {
  prom_counter_t *c = prom_counter_new("test_counter", "counter under test", 2, (const char *[]){"foo", "bar"});
  TEST_ASSERT(c);

  prom_counter_inc(c, sample_labels_a);

  prom_metric_sample_t *sample = prom_metric_sample_from_labels(c, sample_labels_a);
  TEST_ASSERT_EQUAL_DOUBLE(1.0, sample->r_value);

  sample = prom_metric_sample_from_labels(c, sample_labels_b);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, sample->r_value);

  prom_counter_destroy(c);
  c = NULL;
}

void test_counter_add(void) {
  prom_counter_t *c = prom_counter_new("test_counter", "counter under test", 2, (const char *[]){"foo", "bar"});
  TEST_ASSERT(c);

  prom_counter_add(c, 100000000.1, sample_labels_a);
  prom_metric_sample_t *sample = prom_metric_sample_from_labels(c, sample_labels_a);
  TEST_ASSERT_EQUAL_DOUBLE(100000000.1, sample->r_value);

  sample = prom_metric_sample_from_labels(c, sample_labels_b);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, sample->r_value);

  prom_counter_destroy(c);
  c = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_counter_inc);
  RUN_TEST(test_counter_add);
  return UNITY_END();
}

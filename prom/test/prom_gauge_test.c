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

const char *sample_labels_a[] = {"f", "b"};
const char *sample_labels_b[] = {"o", "r"};

void test_gauge_inc(void) {
  prom_gauge_t *g = prom_gauge_new("test_gauge", "gauge under test", 2, (const char *[]){"foo", "bar"});
  TEST_ASSERT(g);

  prom_gauge_inc(g, sample_labels_a);

  prom_metric_sample_t *sample = prom_metric_sample_from_labels(g, sample_labels_a);

  TEST_ASSERT_EQUAL_DOUBLE(1.0, sample->r_value);

  sample = prom_metric_sample_from_labels(g, sample_labels_b);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, sample->r_value);

  prom_gauge_destroy(g);
  g = NULL;
}

void test_gauge_dec(void) {
  prom_gauge_t *g = prom_gauge_new("test_gauge", "gauge under test", 2, (const char *[]){"foo", "bar"});
  TEST_ASSERT(g);

  prom_gauge_dec(g, sample_labels_a);

  prom_metric_sample_t *sample = prom_metric_sample_from_labels(g, sample_labels_a);
  TEST_ASSERT_EQUAL_DOUBLE(-1.0, sample->r_value);

  sample = prom_metric_sample_from_labels(g, sample_labels_b);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, sample->r_value);

  prom_gauge_destroy(g);
  g = NULL;
}

void test_gauge_add(void) {
  prom_gauge_t *g = prom_gauge_new("test_gauge", "gauge under test", 2, (const char *[]){"foo", "bar"});
  TEST_ASSERT(g);

  prom_gauge_add(g, 100000000.1, sample_labels_a);

  prom_metric_sample_t *sample = prom_metric_sample_from_labels(g, sample_labels_a);
  TEST_ASSERT_EQUAL_DOUBLE(100000000.1, sample->r_value);

  sample = prom_metric_sample_from_labels(g, sample_labels_b);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, sample->r_value);

  prom_gauge_destroy(g);
  g = NULL;
}

void test_gauge_sub(void) {
  prom_gauge_t *g = prom_gauge_new("test_gauge", "gauge under test", 2, (const char *[]){"foo", "bar"});
  TEST_ASSERT(g);

  prom_gauge_sub(g, 100000000.1, sample_labels_a);

  prom_metric_sample_t *sample = prom_metric_sample_from_labels(g, sample_labels_a);
  TEST_ASSERT_EQUAL_DOUBLE(-100000000.1, sample->r_value);

  sample = prom_metric_sample_from_labels(g, sample_labels_b);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, sample->r_value);

  prom_gauge_destroy(g);
  g = NULL;
}

void test_gauge_set(void) {
  prom_gauge_t *g = prom_gauge_new("test_gauge", "gauge under test", 2, (const char *[]){"foo", "bar"});
  TEST_ASSERT(g);

  prom_gauge_set(g, 100000000.1, sample_labels_a);

  prom_metric_sample_t *sample = prom_metric_sample_from_labels(g, sample_labels_a);
  TEST_ASSERT_EQUAL_DOUBLE(100000000.1, sample->r_value);

  sample = prom_metric_sample_from_labels(g, sample_labels_b);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, sample->r_value);

  prom_gauge_destroy(g);
  g = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_gauge_inc);
  RUN_TEST(test_gauge_dec);
  RUN_TEST(test_gauge_add);
  RUN_TEST(test_gauge_sub);
  RUN_TEST(test_gauge_set);
  return UNITY_END();
}

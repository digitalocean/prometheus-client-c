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

const char *l_value = "test{foo=\"bar\"}";

void test_prom_metric_sample_add(void) {
  prom_metric_sample_t *s = prom_metric_sample_new(PROM_COUNTER, l_value, 0.0);
  TEST_ASSERT(s);

  prom_metric_sample_add(s, 2.6);
  TEST_ASSERT_EQUAL_DOUBLE(2.6, s->r_value);

  prom_metric_sample_add(s, 29.9);
  TEST_ASSERT_EQUAL_DOUBLE(32.5, s->r_value);

  prom_metric_sample_add(s, 1000001.125);
  TEST_ASSERT_EQUAL_DOUBLE(1000033.625, s->r_value);

  prom_metric_sample_destroy(s);
  s = NULL;
}

void test_prom_metric_sample_sub(void) {
  prom_metric_sample_t *s = prom_metric_sample_new(PROM_GAUGE, l_value, 100.1145321);
  TEST_ASSERT(s);

  prom_metric_sample_sub(s, 99.91);
  TEST_ASSERT_EQUAL_DOUBLE(0.20453210000000865, s->r_value);

  prom_metric_sample_sub(s, 0.20453210000000865);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, s->r_value);

  prom_metric_sample_destroy(s);
  s = NULL;
}

void test_prom_metric_set(void) {
  prom_metric_sample_t *s = prom_metric_sample_new(PROM_GAUGE, l_value, 100.1145321);
  TEST_ASSERT(s);

  prom_metric_sample_set(s, 99.91);
  TEST_ASSERT_EQUAL_DOUBLE(99.91, s->r_value);

  prom_metric_sample_set(s, 0.20453210000000865);
  TEST_ASSERT_EQUAL_DOUBLE(0.20453210000000865, s->r_value);

  prom_metric_sample_destroy(s);
  s = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_metric_sample_add);
  RUN_TEST(test_prom_metric_sample_sub);
  RUN_TEST(test_prom_metric_set);
  return UNITY_END();
}

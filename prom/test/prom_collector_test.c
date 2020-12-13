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

void test_prom_collector(void) {
  prom_collector_t *collector = prom_collector_new("test");
  prom_counter_t *counter = prom_counter_new("test_counter", "counter under test", 0, NULL);
  prom_collector_add_metric(collector, counter);
  prom_map_t *m = collector->collect_fn(collector);
  TEST_ASSERT_EQUAL_INT(1, prom_map_size(m));
  prom_collector_destroy(collector);
  collector = NULL;
}

void test_prom_process_collector(void) {
  prom_collector_t *collector =
      prom_collector_process_new("/code/prom/test/fixtures/limits", "/code/prom/test/fixtures/stat");
  prom_map_t *m = collector->collect_fn(collector);
  TEST_ASSERT_EQUAL_INT(7, prom_map_size(m));
  prom_collector_destroy(collector);
  collector = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_collector);
  RUN_TEST(test_prom_process_collector);
  return UNITY_END();
}

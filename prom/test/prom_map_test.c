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

void test_prom_map(void) {
  prom_map_t *map = prom_map_new();
  prom_map_set(map, "foo", "bar");
  prom_map_set(map, "bing", "bang");
  TEST_ASSERT_EQUAL_STRING("bar", (const char *)prom_map_get(map, "foo"));
  TEST_ASSERT_EQUAL_STRING("bang", (const char *)prom_map_get(map, "bing"));
  TEST_ASSERT_NULL(prom_map_get(map, "nope"));
  TEST_ASSERT_EQUAL_INT(2, prom_map_size(map));

  for (prom_linked_list_node_t *current_node = map->keys->head; current_node != NULL;
       current_node = current_node->next) {
    const char *key = (const char *)current_node->item;

    const char *expected;
    if (strcmp(key, "foo") == 0) {
      expected = "bar";
    } else if (strcmp(key, "bing") == 0) {
      expected = "bang";
    }

    const char *result = prom_map_get(map, key);

    TEST_ASSERT_EQUAL_STRING(expected, result);
  }

  prom_map_destroy(map);
  map = NULL;
}

void test_prom_map_when_large(void) {
  prom_map_t *map = prom_map_new();
  prom_map_set_free_value_fn(map, free);

  // Ensure each inserted key and value are present
  for (int i = 1; i <= 10000; i++) {
    char buf[4];
    sprintf(buf, "%d", i);
    const char *k = (const char *)buf;
    int *set = malloc(sizeof(int));
    *set = i;
    prom_map_set(map, k, (void *)set);
    TEST_ASSERT_EQUAL_INT(i, *((int *)prom_map_get(map, k)));
  }

  // Update one of the keys
  int *at_50 = malloc(sizeof(int));
  *at_50 = 5000;
  prom_map_set(map, "50", (void *)at_50);
  TEST_ASSERT_EQUAL_INT(5000, *((int *)prom_map_get(map, "50")));

  // Ensure each key and value is correct
  for (int i = 1; i <= 10000; i++) {
    char buf[5];
    sprintf(buf, "%d", i);
    const char *k = (const char *)buf;
    int actual = *((int *)prom_map_get(map, k));
    if (i == 50) {
      TEST_ASSERT_EQUAL_INT(5000, actual);
    } else {
      TEST_ASSERT_EQUAL_INT(i, actual);
    }
  }

  // Ensure the size and max_size are correct
  TEST_ASSERT_EQUAL_INT(10000, map->size);
  TEST_ASSERT_EQUAL_INT(32768, map->max_size);

  prom_map_destroy(map);
  map = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_map);
  RUN_TEST(test_prom_map_when_large);
  return UNITY_END();
}

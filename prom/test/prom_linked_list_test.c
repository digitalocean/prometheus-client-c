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

void test_prom_linked_list_append(void) {
  prom_linked_list_t *l = prom_linked_list_new();
  TEST_ASSERT(l);
  prom_linked_list_set_free_fn(l, prom_linked_list_no_op_free);

  char *one = "one";
  char *two = "two";
  char *three = "three";

  prom_linked_list_append(l, one);
  prom_linked_list_append(l, two);
  prom_linked_list_append(l, three);

  prom_linked_list_node_t *current_node = l->head;
  char *current_str = (char *)current_node->item;
  TEST_ASSERT_EQUAL_STRING("one", current_str);
  current_node = current_node->next;
  current_str = current_node->item;
  TEST_ASSERT_EQUAL_STRING("two", current_str);
  current_node = current_node->next;
  current_str = current_node->item;
  TEST_ASSERT_EQUAL_STRING("three", current_str);

  prom_linked_list_destroy(l);
  l = NULL;
}

void test_prom_linked_list_push(void) {
  prom_linked_list_t *l = prom_linked_list_new();
  TEST_ASSERT(l);
  prom_linked_list_set_free_fn(l, prom_linked_list_no_op_free);

  char *one = "one";
  char *two = "two";
  char *three = "three";

  prom_linked_list_push(l, one);
  prom_linked_list_push(l, two);
  prom_linked_list_push(l, three);

  prom_linked_list_node_t *current_node = l->head;
  char *current_str = (char *)current_node->item;
  TEST_ASSERT_EQUAL_STRING("three", current_str);
  current_node = current_node->next;
  current_str = current_node->item;
  TEST_ASSERT_EQUAL_STRING("two", current_str);
  current_node = current_node->next;
  current_str = current_node->item;
  TEST_ASSERT_EQUAL_STRING("one", current_str);

  prom_linked_list_destroy(l);
  l = NULL;
}

void test_prom_linked_list_remove(void) {
  auto prom_linked_list_compare_t compare_fn(void *item_a, void *item_b) {
    const char *str_a = (const char *)item_a;
    const char *str_b = (const char *)item_b;
    return strcmp(str_a, str_b);
  }

  prom_linked_list_t *list = prom_linked_list_new();
  prom_linked_list_set_free_fn(list, prom_linked_list_no_op_free);
  prom_linked_list_set_compare_fn(list, compare_fn);

  prom_linked_list_append(list, "node_a");
  prom_linked_list_append(list, "node_b");
  prom_linked_list_append(list, "node_c");

  prom_linked_list_remove(list, "node_b");
  const char *result_a = list->head->item;
  TEST_ASSERT_EQUAL_STRING("node_a", result_a);
  const char *result_c = list->head->next->item;
  TEST_ASSERT_EQUAL_STRING("node_c", result_c);

  prom_linked_list_destroy(list);
  list = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_linked_list_append);
  RUN_TEST(test_prom_linked_list_push);
  RUN_TEST(test_prom_linked_list_remove);
  return UNITY_END();
}

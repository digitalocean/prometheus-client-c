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

void test_prom_string_builder_add_str(void) {
  prom_string_builder_t *sb = prom_string_builder_new();
  prom_string_builder_add_str(sb, "fooooooooooooooooooooooooooooooooo");
  prom_string_builder_add_str(sb, " baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaar");
  TEST_ASSERT_EQUAL_STRING(prom_string_builder_str(sb),
                           "fooooooooooooooooooooooooooooooooo baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaar");

  prom_string_builder_destroy(sb);
  sb = NULL;
}

void test_prom_string_builder_add_char(void) {
  prom_string_builder_t *sb = prom_string_builder_new();
  const char *foobar = "foo bar";
  for (int i = 0; i < strlen(foobar); i++) {
    prom_string_builder_add_char(sb, foobar[i]);
  }
  TEST_ASSERT_EQUAL_STRING(prom_string_builder_str(sb), "foo bar");

  prom_string_builder_destroy(sb);
  sb = NULL;
}

void test_prom_string_builder_dump(void) {
  prom_string_builder_t *sb = prom_string_builder_new();
  const char *original = "foo bar";
  prom_string_builder_add_str(sb, original);
  const char *result = prom_string_builder_dump(sb);
  prom_string_builder_clear(sb);
  TEST_ASSERT(original != result);
  TEST_ASSERT_EQUAL_STRING(original, result);

  prom_string_builder_destroy(sb);
  free((char *)result);
  result = NULL;
  sb = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_string_builder_add_str);
  RUN_TEST(test_prom_string_builder_add_char);
  RUN_TEST(test_prom_string_builder_dump);
  return UNITY_END();
}

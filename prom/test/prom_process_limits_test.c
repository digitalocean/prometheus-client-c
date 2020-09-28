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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prom_map_i.h"
#include "prom_process_limits_i.h"
#include "prom_process_limits_t.h"
#include "unity.h"

const char *path = "/code/prom/test/fixtures/limits";

void test_prom_process_limits_file_parsing(void) {
  prom_process_limits_file_t *f = prom_process_limits_file_new(path);
  char *result = strstr(f->buf, "Max realtime timeout");
  TEST_ASSERT_NOT_NULL(result);

  prom_map_t *m = prom_process_limits(f);
  if (!m) TEST_FAIL();

  TEST_ASSERT_EQUAL_INT(16, prom_map_size(m));

  prom_process_limits_row_t *row = (prom_process_limits_row_t *)prom_map_get(m, "Max cpu time");
  if (!row) TEST_FAIL();
  TEST_ASSERT_EQUAL_INT(-1, row->soft);
  TEST_ASSERT_EQUAL_INT(-1, row->hard);
  TEST_ASSERT_EQUAL_STRING("Max cpu time", row->limit);
  TEST_ASSERT_EQUAL_STRING("seconds", row->units);

  row = (prom_process_limits_row_t *)prom_map_get(m, "Max file size");
  TEST_ASSERT_EQUAL_INT(-1, row->soft);
  TEST_ASSERT_EQUAL_INT(-1, row->hard);
  TEST_ASSERT_EQUAL_STRING("Max file size", row->limit);
  TEST_ASSERT_EQUAL_STRING("bytes", row->units);

  row = (prom_process_limits_row_t *)prom_map_get(m, "Max data size");
  TEST_ASSERT_EQUAL_INT(-1, row->soft);
  TEST_ASSERT_EQUAL_INT(-1, row->hard);
  TEST_ASSERT_EQUAL_STRING("Max data size", row->limit);
  TEST_ASSERT_EQUAL_STRING("bytes", row->units);

  row = (prom_process_limits_row_t *)prom_map_get(m, "Max stack size");
  TEST_ASSERT_EQUAL_INT(8388608, row->soft);
  TEST_ASSERT_EQUAL_INT(-1, row->hard);
  TEST_ASSERT_EQUAL_STRING("Max stack size", row->limit);
  TEST_ASSERT_EQUAL_STRING("bytes", row->units);

  row = (prom_process_limits_row_t *)prom_map_get(m, "Max processes");
  TEST_ASSERT_EQUAL_INT(-1, row->soft);
  TEST_ASSERT_EQUAL_INT(-1, row->hard);
  TEST_ASSERT_EQUAL_STRING("Max processes", row->limit);
  TEST_ASSERT_EQUAL_STRING("processes", row->units);

  row = (prom_process_limits_row_t *)prom_map_get(m, "Max pending signals");
  TEST_ASSERT_EQUAL_INT(23701, row->soft);
  TEST_ASSERT_EQUAL_INT(23701, row->hard);
  TEST_ASSERT_EQUAL_STRING("Max pending signals", row->limit);
  TEST_ASSERT_EQUAL_STRING("signals", row->units);

  prom_map_destroy(m);
  m = NULL;
  prom_process_limits_file_destroy(f);
  f = NULL;
}

void test_prom_process_limits_rdp_next_token(void) {
  prom_process_limits_file_t f = {.size = 4, .index = 0, .buf = " \t!"};
  prom_process_limits_file_t *fp = &f;
  prom_process_limits_rdp_next_token(fp);

  TEST_ASSERT_EQUAL_INT(2, fp->index);
  TEST_ASSERT_EQUAL_INT('!', fp->buf[fp->index]);
}

void test_prom_process_limits_rdp_match(void) {
  prom_process_limits_file_t f = {.size = 4, .index = 0, .buf = "foo"};
  prom_process_limits_file_t *fp = &f;

  TEST_ASSERT_TRUE(prom_process_limits_rdp_match(fp, "foo"));
}

void test_prom_process_limits_rdp_hard_limit(void) {
  // Test unlimited value
  prom_process_limits_file_t f = {.size = 13, .index = 0, .buf = "unlimited   "};
  prom_process_limits_file_t *fp = &f;

  prom_process_limits_current_row_t *cr = prom_process_limits_current_row_new();
  prom_map_t *m = prom_map_new();

  TEST_ASSERT_TRUE(prom_process_limits_rdp_hard_limit(fp, m, cr));
  TEST_ASSERT_EQUAL_INT(-1, cr->hard);

  prom_process_limits_current_row_clear(cr);

  // Test int value
  fp->buf = "123  ";
  fp->size = 6;
  fp->index = 0;

  TEST_ASSERT_TRUE(prom_process_limits_rdp_hard_limit(fp, m, cr));
  TEST_ASSERT_EQUAL_INT(123, cr->hard);

  prom_map_destroy(m);
  m = NULL;
  prom_process_limits_current_row_destroy(cr);
  cr = NULL;
}

void test_prom_process_limits_rdp_word(void) {
  // Test unlimited value
  prom_process_limits_file_t f = {.size = 13, .index = 0, .buf = "unlimited   "};
  prom_process_limits_file_t *fp = &f;

  prom_process_limits_current_row_t *cr = prom_process_limits_current_row_new();
  prom_map_t *m = prom_map_new();

  TEST_ASSERT_TRUE(prom_process_limits_rdp_word(fp, m, cr));
  TEST_ASSERT_EQUAL_INT(9, fp->index);

  prom_map_destroy(m);
  m = NULL;
  prom_process_limits_current_row_destroy(cr);
  cr = NULL;
}

void test_prom_process_limits_rdp_word_and_space(void) {
  prom_process_limits_file_t f = {.size = 8, .index = 0, .buf = "foo bar"};
  prom_process_limits_file_t *fp = &f;

  prom_process_limits_current_row_t *cr = prom_process_limits_current_row_new();
  prom_map_t *m = prom_map_new();

  TEST_ASSERT_TRUE(prom_process_limits_rdp_word_and_space(fp, m, cr));
  TEST_ASSERT_EQUAL_INT(4, fp->index);
  TEST_ASSERT_EQUAL_INT('b', fp->buf[fp->index]);

  prom_map_destroy(m);
  m = NULL;
  prom_process_limits_current_row_destroy(cr);
  cr = NULL;
}

void test_prom_process_limits_rdp_limit(void) {
  prom_process_limits_file_t f = {.size = 16, .index = 0, .buf = "Max cpu time   "};
  prom_process_limits_file_t *fp = &f;

  prom_process_limits_current_row_t *cr = prom_process_limits_current_row_new();
  prom_map_t *m = prom_map_new();

  TEST_ASSERT_TRUE(prom_process_limits_rdp_limit(fp, m, cr));
  TEST_ASSERT_EQUAL_INT(12, fp->index);
  TEST_ASSERT_EQUAL_INT(' ', fp->buf[fp->index]);
  TEST_ASSERT_EQUAL_STRING("Max cpu time", cr->limit);

  prom_map_destroy(m);
  m = NULL;
  prom_process_limits_current_row_destroy(cr);
  cr = NULL;
}

void test_prom_process_limits_rdp_letter(void) {
  prom_process_limits_file_t f = {.size = 4, .index = 0, .buf = "foo"};
  prom_process_limits_file_t *fp = &f;

  prom_process_limits_current_row_t *cr = prom_process_limits_current_row_new();
  prom_map_t *m = prom_map_new();

  TEST_ASSERT_TRUE(prom_process_limits_rdp_letter(fp, m, cr));

  fp->size = 1;
  fp->index = 0;
  fp->buf = "";

  TEST_ASSERT_FALSE(prom_process_limits_rdp_letter(fp, m, cr));

  fp->size = 2;
  fp->index = 0;
  fp->buf = "2";

  TEST_ASSERT_FALSE(prom_process_limits_rdp_letter(fp, m, cr));

  prom_map_destroy(m);
  m = NULL;
  prom_process_limits_current_row_destroy(cr);
  cr = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_process_limits_rdp_next_token);
  RUN_TEST(test_prom_process_limits_rdp_match);
  RUN_TEST(test_prom_process_limits_rdp_hard_limit);
  RUN_TEST(test_prom_process_limits_rdp_word);
  RUN_TEST(test_prom_process_limits_rdp_word_and_space);
  RUN_TEST(test_prom_process_limits_rdp_limit);
  RUN_TEST(test_prom_process_limits_rdp_letter);

  RUN_TEST(test_prom_process_limits_file_parsing);
  return UNITY_END();
}

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

void test_prom_procfs_buf(void) {
  prom_procfs_buf_t *buf = prom_procfs_buf_new("/proc/1/limits");
  TEST_ASSERT_NOT_NULL(buf);
  TEST_ASSERT_NOT_NULL(strstr(buf->buf, "Max realtime timeout"));
  prom_procfs_buf_destroy(buf);
  buf = NULL;
}

int main(int argc, const char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prom_procfs_buf);
  return UNITY_END();
}

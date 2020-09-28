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

#ifndef PROM_LOG_H
#define PROM_LOG_H

#ifdef PROM_LOG_ENABLE
#define PROM_LOG(msg) printf("%s %s %s %s %d %s\n", __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__, msg);
#else
#define PROM_LOG(msg)
#endif  // PROM_LOG_ENABLE

#endif  // PROM_LOG_H

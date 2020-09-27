/*
Copyright 2019-2020 DigitalOcean Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/**
 * @file prom_alloc.h
 * @brief memory management
 */

#ifndef PROM_ALLOC_H
#define PROM_ALLOC_H

#include <stdlib.h>
#include <string.h>

/**
 * @brief Redefine this macro if you wish to override it. The default value is malloc.
 */
#define prom_malloc malloc

/**
 * @brief Redefine this macro if you wish to override it. The default value is realloc.
 */
#define prom_realloc realloc

/**
 * @brief Redefine this macro if you wish to override it. The default value is strdup.
 */
#define prom_strdup strdup

/**
 * @brief Redefine this macro if you wish to override it. The default value is free.
 */
#define prom_free free

#endif  // PROM_ALLOC_H

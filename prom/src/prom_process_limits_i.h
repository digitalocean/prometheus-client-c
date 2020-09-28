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

#ifndef PROM_PROCESS_I_H
#define PROM_PROCESS_I_H

#include <stdbool.h>

#include "prom_map_t.h"
#include "prom_process_limits_t.h"

/**
 * @brief Initialize the process gauge metrics
 */
int prom_process_init(void);

prom_process_limits_row_t *prom_process_limits_row_new(const char *limit, const int soft, const int hard,
                                                       const char *units);
int prom_process_limits_row_destroy(prom_process_limits_row_t *self);

prom_process_limits_current_row_t *prom_process_limits_current_row_new(void);
int prom_process_limits_current_row_set_limit(prom_process_limits_current_row_t *self, char *limit);
int prom_process_limits_current_row_set_units(prom_process_limits_current_row_t *self, char *units);
int prom_process_limits_current_row_clear(prom_process_limits_current_row_t *self);
int prom_process_limits_current_row_destroy(prom_process_limits_current_row_t *self);

prom_process_limits_file_t *prom_process_limits_file_new(const char *path);
int prom_process_limits_file_destroy(prom_process_limits_file_t *self);

prom_map_t *prom_process_limits(prom_process_limits_file_t *f);
bool prom_process_limits_rdp_file(prom_process_limits_file_t *f, prom_map_t *data,
                                  prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_first_line(prom_process_limits_file_t *f, prom_map_t *data,
                                        prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_character(prom_process_limits_file_t *f, prom_map_t *data,
                                       prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_letter(prom_process_limits_file_t *f, prom_map_t *data,
                                    prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_digit(prom_process_limits_file_t *f, prom_map_t *data,
                                   prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_data_line(prom_process_limits_file_t *f, prom_map_t *data,
                                       prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_limit(prom_process_limits_file_t *f, prom_map_t *data,
                                   prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_word_and_space(prom_process_limits_file_t *f, prom_map_t *data,
                                            prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_word(prom_process_limits_file_t *f, prom_map_t *data,
                                  prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_soft_limit(prom_process_limits_file_t *f, prom_map_t *data,
                                        prom_process_limits_current_row_t *current_row);
;
bool prom_process_limits_rdp_hard_limit(prom_process_limits_file_t *f, prom_map_t *data,
                                        prom_process_limits_current_row_t *current_row);
bool prom_process_limits_rdp_units(prom_process_limits_file_t *f, prom_map_t *data,
                                   prom_process_limits_current_row_t *current_row);

int prom_process_limits_rdp_next_token(prom_process_limits_file_t *f);
bool prom_process_limits_rdp_match(prom_process_limits_file_t *f, const char *token);

int prom_process_limits_init(void);

#endif  // PROM_PROCESS_I_H

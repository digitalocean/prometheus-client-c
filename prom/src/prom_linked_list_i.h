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

#ifndef PROM_LIST_I_INCLUDED
#define PROM_LIST_I_INCLUDED

// Private
#include "prom_linked_list_t.h"

/**
 * @brief API PRIVATE Returns a pointer to a prom_linked_list
 */
prom_linked_list_t *prom_linked_list_new(void);

/**
 * @brief API PRIVATE removes all nodes from the given prom_linked_list *
 */
int prom_linked_list_purge(prom_linked_list_t *self);

/**
 * @brief API PRIVATE Destroys a prom_linked_list
 */
int prom_linked_list_destroy(prom_linked_list_t *self);

/**
 * @brief API PRIVATE Append an item to the back of the list
 */
int prom_linked_list_append(prom_linked_list_t *self, void *item);

/**
 * @brief API PRIVATE Push an item onto the front of the list
 */
int prom_linked_list_push(prom_linked_list_t *self, void *item);

/**
 * @brief API PRIVATE Pop the first item off of the list
 */
void *prom_linked_list_pop(prom_linked_list_t *self);

/**
 * @brief API PRIVATE Returns the item at the head of the list or NULL if not present
 */
void *prom_linked_list_first(prom_linked_list_t *self);

/**
 * @brief API PRIVATE Returns the item at the tail of the list or NULL if not present
 */
void *prom_linked_list_last(prom_linked_list_t *self);

/**
 * @brief API PRIVATE Removes an item from the linked list
 */
int prom_linked_list_remove(prom_linked_list_t *self, void *item);

/**
 * @brief API PRIVATE Compares two items within a linked list
 */
prom_linked_list_compare_t prom_linked_list_compare(prom_linked_list_t *self, void *item_a, void *node_b);

/**
 * @brief API PRIVATE Get the size
 */
size_t prom_linked_list_size(prom_linked_list_t *self);

/**
 * @brief API PRIVATE Set the free_fn member on prom_linked_list
 */
int prom_linked_list_set_free_fn(prom_linked_list_t *self, prom_linked_list_free_item_fn free_fn);

/**
 * @brief API PRIVATE Set the compare_fn member on the prom_linked_list
 */
int prom_linked_list_set_compare_fn(prom_linked_list_t *self, prom_linked_list_compare_item_fn compare_fn);

/**
 * API PRIVATE
 * @brief does nothing
 */
void prom_linked_list_no_op_free(void *item);

#endif  // PROM_LIST_I_INCLUDED

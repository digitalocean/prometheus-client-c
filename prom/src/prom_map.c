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

#include <pthread.h>
#include <stdbool.h>

// Public
#include "prom_alloc.h"

// Private
#include "prom_assert.h"
#include "prom_errors.h"
#include "prom_linked_list_i.h"
#include "prom_linked_list_t.h"
#include "prom_log.h"
#include "prom_map_i.h"
#include "prom_map_t.h"

#define PROM_MAP_INITIAL_SIZE 32

static void destroy_map_node_value_no_op(void *value) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// prom_map_node
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

prom_map_node_t *prom_map_node_new(const char *key, void *value, prom_map_node_free_value_fn free_value_fn) {
  prom_map_node_t *self = prom_malloc(sizeof(prom_map_node_t));
  self->key = prom_strdup(key);
  self->value = value;
  self->free_value_fn = free_value_fn;
  return self;
}

int prom_map_node_destroy(prom_map_node_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 0;
  prom_free((void *)self->key);
  self->key = NULL;
  if (self->value != NULL) (*self->free_value_fn)(self->value);
  self->value = NULL;
  prom_free(self);
  self = NULL;
  return 0;
}

void prom_map_node_free(void *item) {
  prom_map_node_t *map_node = (prom_map_node_t *)item;
  prom_map_node_destroy(map_node);
}

prom_linked_list_compare_t prom_map_node_compare(void *item_a, void *item_b) {
  prom_map_node_t *map_node_a = (prom_map_node_t *)item_a;
  prom_map_node_t *map_node_b = (prom_map_node_t *)item_b;

  return strcmp(map_node_a->key, map_node_b->key);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// prom_map
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

prom_map_t *prom_map_new() {
  int r = 0;

  prom_map_t *self = (prom_map_t *)prom_malloc(sizeof(prom_map_t));
  self->size = 0;
  self->max_size = PROM_MAP_INITIAL_SIZE;

  self->keys = prom_linked_list_new();
  if (self->keys == NULL) return NULL;

  // These each key will be allocated once by prom_map_node_new and used here as well to save memory. With that said
  // we will only have to deallocate each key once. That will happen on prom_map_node_destroy.
  r = prom_linked_list_set_free_fn(self->keys, prom_linked_list_no_op_free);
  if (r) {
    prom_map_destroy(self);
    return NULL;
  }

  self->addrs = prom_malloc(sizeof(prom_linked_list_t) * self->max_size);
  self->free_value_fn = destroy_map_node_value_no_op;

  for (int i = 0; i < self->max_size; i++) {
    self->addrs[i] = prom_linked_list_new();
    r = prom_linked_list_set_free_fn(self->addrs[i], prom_map_node_free);
    if (r) {
      prom_map_destroy(self);
      return NULL;
    }
    r = prom_linked_list_set_compare_fn(self->addrs[i], prom_map_node_compare);
    if (r) {
      prom_map_destroy(self);
      return NULL;
    }
  }

  self->rwlock = (pthread_rwlock_t *)prom_malloc(sizeof(pthread_rwlock_t));
  r = pthread_rwlock_init(self->rwlock, NULL);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_INIT_ERROR);
    prom_map_destroy(self);
    return NULL;
  }

  return self;
}

int prom_map_destroy(prom_map_t *self) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  int ret = 0;

  r = prom_linked_list_destroy(self->keys);
  if (r) ret = r;
  self->keys = NULL;

  for (size_t i = 0; i < self->max_size; i++) {
    r = prom_linked_list_destroy(self->addrs[i]);
    if (r) ret = r;
    self->addrs[i] = NULL;
  }
  prom_free(self->addrs);
  self->addrs = NULL;

  r = pthread_rwlock_destroy(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_DESTROY_ERROR)
    ret = r;
  }

  prom_free(self->rwlock);
  self->rwlock = NULL;
  prom_free(self);
  self = NULL;

  return ret;
}

static size_t prom_map_get_index_internal(const char *key, size_t *size, size_t *max_size) {
  size_t index;
  size_t a = 31415, b = 27183;
  for (index = 0; *key != '\0'; key++, a = a * b % (*max_size - 1)) {
    index = (a * index + *key) % *max_size;
  }
  return index;
}

/**
 * @brief API PRIVATE hash function that returns an array index from the given key and prom_map.
 *
 * The algorithm is based off of Horner's method. In a simpler version, you set the return value to 0. Next, for each
 * character in the string, you add the integer value of the current character to the product of the prime number and
 * the current return value, set the result to the return value, then finally return the return value.
 *
 * In this version of the algorithm, we attempt to achieve a probabily of key to index conversion collisions to
 * 1/M (with M being the max_size of the map). This optimizes dispersion and consequently, evens out the performance
 * for gets and sets for each item. Instead of using a fixed prime number, we generate a coefficient for each iteration
 * through the loop.
 *
 * Reference:
 *   * Algorithms in C: Third Edition by Robert Sedgewick, p579
 */
size_t prom_map_get_index(prom_map_t *self, const char *key) {
  return prom_map_get_index_internal(key, &self->size, &self->max_size);
}

static void *prom_map_get_internal(const char *key, size_t *size, size_t *max_size, prom_linked_list_t *keys,
                                   prom_linked_list_t **addrs, prom_map_node_free_value_fn free_value_fn) {
  size_t index = prom_map_get_index_internal(key, size, max_size);
  prom_linked_list_t *list = addrs[index];
  prom_map_node_t *temp_map_node = prom_map_node_new(key, NULL, free_value_fn);

  for (prom_linked_list_node_t *current_node = list->head; current_node != NULL; current_node = current_node->next) {
    prom_map_node_t *current_map_node = (prom_map_node_t *)current_node->item;
    prom_linked_list_compare_t result = prom_linked_list_compare(list, current_map_node, temp_map_node);
    if (result == PROM_EQUAL) {
      prom_map_node_destroy(temp_map_node);
      temp_map_node = NULL;
      return current_map_node->value;
    }
  }
  prom_map_node_destroy(temp_map_node);
  temp_map_node = NULL;
  return NULL;
}

void *prom_map_get(prom_map_t *self, const char *key) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  r = pthread_rwlock_wrlock(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_LOCK_ERROR);
    NULL;
  }
  void *payload =
      prom_map_get_internal(key, &self->size, &self->max_size, self->keys, self->addrs, self->free_value_fn);
  r = pthread_rwlock_unlock(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);
    return NULL;
  }
  return payload;
}

static int prom_map_set_internal(const char *key, void *value, size_t *size, size_t *max_size, prom_linked_list_t *keys,
                                 prom_linked_list_t **addrs, prom_map_node_free_value_fn free_value_fn,
                                 bool destroy_current_value) {
  prom_map_node_t *map_node = prom_map_node_new(key, value, free_value_fn);
  if (map_node == NULL) return 1;

  size_t index = prom_map_get_index_internal(key, size, max_size);
  prom_linked_list_t *list = addrs[index];
  for (prom_linked_list_node_t *current_node = list->head; current_node != NULL; current_node = current_node->next) {
    prom_map_node_t *current_map_node = (prom_map_node_t *)current_node->item;
    prom_linked_list_compare_t result = prom_linked_list_compare(list, current_map_node, map_node);
    if (result == PROM_EQUAL) {
      if (destroy_current_value) {
        free_value_fn(current_map_node->value);
        current_map_node->value = NULL;
      }
      prom_free((char *)current_map_node->key);
      current_map_node->key = NULL;
      prom_free(current_map_node);
      current_map_node = NULL;
      current_node->item = map_node;
      return 0;
    }
  }
  prom_linked_list_append(list, map_node);
  prom_linked_list_append(keys, (char *)map_node->key);
  (*size)++;
  return 0;
}

int prom_map_ensure_space(prom_map_t *self) {
  PROM_ASSERT(self != NULL);
  int r = 0;

  if (self->size <= self->max_size / 2) {
    return 0;
  }

  // Increase the max size
  size_t new_max = self->max_size * 2;
  size_t new_size = 0;

  // Create a new list of keys
  prom_linked_list_t *new_keys = prom_linked_list_new();
  if (new_keys == NULL) return 1;

  r = prom_linked_list_set_free_fn(new_keys, prom_linked_list_no_op_free);
  if (r) return r;

  // Create a new array of addrs
  prom_linked_list_t **new_addrs = prom_malloc(sizeof(prom_linked_list_t) * new_max);

  // Initialize the new array
  for (int i = 0; i < new_max; i++) {
    new_addrs[i] = prom_linked_list_new();
    r = prom_linked_list_set_free_fn(new_addrs[i], prom_map_node_free);
    if (r) return r;
    r = prom_linked_list_set_compare_fn(new_addrs[i], prom_map_node_compare);
    if (r) return r;
  }

  // Iterate through each linked-list at each memory region in the map's backbone
  for (int i = 0; i < self->max_size; i++) {
    // Create a new map node for each node in the linked list and insert it into the new map. Afterwards, deallocate
    // the old map node
    prom_linked_list_t *list = self->addrs[i];
    prom_linked_list_node_t *current_node = list->head;
    while (current_node != NULL) {
      prom_map_node_t *map_node = (prom_map_node_t *)current_node->item;
      r = prom_map_set_internal(map_node->key, map_node->value, &new_size, &new_max, new_keys, new_addrs,
                                self->free_value_fn, false);
      if (r) return r;

      prom_linked_list_node_t *next = current_node->next;
      prom_free(current_node);
      current_node = NULL;
      prom_free((void *)map_node->key);
      map_node->key = NULL;
      prom_free(map_node);
      map_node = NULL;
      current_node = next;
    }
    // We're done deallocating each map node in the linked list, so deallocate the linked-list object
    prom_free(self->addrs[i]);
    self->addrs[i] = NULL;
  }
  // Destroy the collection of keys in the map
  prom_linked_list_destroy(self->keys);
  self->keys = NULL;

  // Deallocate the backbone of the map
  prom_free(self->addrs);
  self->addrs = NULL;

  // Update the members of the current map
  self->size = new_size;
  self->max_size = new_max;
  self->keys = new_keys;
  self->addrs = new_addrs;

  return 0;
}

int prom_map_set(prom_map_t *self, const char *key, void *value) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  r = pthread_rwlock_wrlock(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_LOCK_ERROR);
    return r;
  }

  r = prom_map_ensure_space(self);
  if (r) {
    int rr = 0;
    rr = pthread_rwlock_unlock(self->rwlock);
    if (rr) {
      PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);
      return rr;
    } else {
      return r;
    }
  }
  r = prom_map_set_internal(key, value, &self->size, &self->max_size, self->keys, self->addrs, self->free_value_fn,
                            true);
  if (r) {
    int rr = 0;
    rr = pthread_rwlock_unlock(self->rwlock);
    if (rr) {
      PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);
      return rr;
    } else {
      return r;
    }
  }
  r = pthread_rwlock_unlock(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);
  }
  return r;
}

static int prom_map_delete_internal(const char *key, size_t *size, size_t *max_size, prom_linked_list_t *keys,
                                    prom_linked_list_t **addrs, prom_map_node_free_value_fn free_value_fn) {
  int r = 0;
  size_t index = prom_map_get_index_internal(key, size, max_size);
  prom_linked_list_t *list = addrs[index];
  prom_map_node_t *temp_map_node = prom_map_node_new(key, NULL, free_value_fn);

  for (prom_linked_list_node_t *current_node = list->head; current_node != NULL; current_node = current_node->next) {
    prom_map_node_t *current_map_node = (prom_map_node_t *)current_node->item;
    prom_linked_list_compare_t result = prom_linked_list_compare(list, current_map_node, temp_map_node);
    if (result == PROM_EQUAL) {
      r = prom_linked_list_remove(list, current_node);
      if (r) return r;

      r = prom_linked_list_remove(keys, (char *)current_map_node->key);
      if (r) return r;

      (*size)--;
      break;
    }
  }
  r = prom_map_node_destroy(temp_map_node);
  temp_map_node = NULL;
  return r;
}

int prom_map_delete(prom_map_t *self, const char *key) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  int ret = 0;
  r = pthread_rwlock_wrlock(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_LOCK_ERROR);
    ret = r;
  }
  r = prom_map_delete_internal(key, &self->size, &self->max_size, self->keys, self->addrs, self->free_value_fn);
  if (r) ret = r;
  r = pthread_rwlock_unlock(self->rwlock);
  if (r) {
    PROM_LOG(PROM_PTHREAD_RWLOCK_UNLOCK_ERROR);
    ret = r;
  }
  return ret;
}

int prom_map_set_free_value_fn(prom_map_t *self, prom_map_node_free_value_fn free_value_fn) {
  PROM_ASSERT(self != NULL);
  self->free_value_fn = free_value_fn;
  return 0;
}

size_t prom_map_size(prom_map_t *self) {
  PROM_ASSERT(self != NULL);
  return self->size;
}

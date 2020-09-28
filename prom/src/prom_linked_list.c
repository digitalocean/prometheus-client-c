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

// Public
#include "prom_alloc.h"

// Private
#include "prom_assert.h"
#include "prom_linked_list_i.h"
#include "prom_linked_list_t.h"
#include "prom_log.h"

prom_linked_list_t *prom_linked_list_new(void) {
  prom_linked_list_t *self = (prom_linked_list_t *)prom_malloc(sizeof(prom_linked_list_t));
  self->head = NULL;
  self->tail = NULL;
  self->free_fn = NULL;
  self->compare_fn = NULL;
  self->size = 0;
  return self;
}

int prom_linked_list_purge(prom_linked_list_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  prom_linked_list_node_t *node = self->head;
  while (node != NULL) {
    prom_linked_list_node_t *next = node->next;
    if (node->item != NULL) {
      if (self->free_fn) {
        (*self->free_fn)(node->item);
      } else {
        prom_free(node->item);
      }
    }
    prom_free(node);
    node = NULL;
    node = next;
  }
  self->head = NULL;
  self->tail = NULL;
  self->size = 0;
  return 0;
}

int prom_linked_list_destroy(prom_linked_list_t *self) {
  PROM_ASSERT(self != NULL);
  int r = 0;
  int ret = 0;

  r = prom_linked_list_purge(self);
  if (r) ret = r;
  prom_free(self);
  self = NULL;
  return ret;
}

void *prom_linked_list_first(prom_linked_list_t *self) {
  PROM_ASSERT(self != NULL);
  if (self->head) {
    return self->head->item;
  } else {
    return NULL;
  }
}

void *prom_linked_list_last(prom_linked_list_t *self) {
  PROM_ASSERT(self != NULL);
  if (self->tail) {
    return self->tail->item;
  } else {
    return NULL;
  }
}

int prom_linked_list_append(prom_linked_list_t *self, void *item) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  prom_linked_list_node_t *node = (prom_linked_list_node_t *)prom_malloc(sizeof(prom_linked_list_node_t));

  node->item = item;
  if (self->tail) {
    self->tail->next = node;
  } else {
    self->head = node;
  }
  self->tail = node;
  node->next = NULL;
  self->size++;
  return 0;
}

int prom_linked_list_push(prom_linked_list_t *self, void *item) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  prom_linked_list_node_t *node = (prom_linked_list_node_t *)prom_malloc(sizeof(prom_linked_list_node_t));

  node->item = item;
  node->next = self->head;
  self->head = node;
  if (self->tail == NULL) {
    self->tail = node;
  }
  self->size++;
  return 0;
}

void *prom_linked_list_pop(prom_linked_list_t *self) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return NULL;
  prom_linked_list_node_t *node = self->head;
  void *item = NULL;
  if (node != NULL) {
    item = node->item;
    self->head = node->next;
    if (self->tail == node) {
      self->tail = NULL;
    }
    if (node->item != NULL) {
      if (self->free_fn) {
        (*self->free_fn)(node->item);
      } else {
        prom_free(node->item);
      }
    }
    node->item = NULL;
    node = NULL;
    self->size--;
  }
  return item;
}

int prom_linked_list_remove(prom_linked_list_t *self, void *item) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  prom_linked_list_node_t *node;
  prom_linked_list_node_t *prev_node = NULL;

  // Locate the node
  for (node = self->head; node != NULL; node = node->next) {
    if (self->compare_fn) {
      if ((*self->compare_fn)(node->item, item) == PROM_EQUAL) {
        break;
      }
    } else {
      if (node->item == item) {
        break;
      }
    }
    prev_node = node;
  }

  if (node == NULL) return 0;

  if (prev_node) {
    prev_node->next = node->next;
  } else {
    self->head = node->next;
  }
  if (node->next == NULL) {
    self->tail = prev_node;
  }

  if (node->item != NULL) {
    if (self->free_fn) {
      (*self->free_fn)(node->item);
    } else {
      prom_free(node->item);
    }
  }

  node->item = NULL;
  prom_free(node);
  node = NULL;
  self->size--;
  return 0;
}

prom_linked_list_compare_t prom_linked_list_compare(prom_linked_list_t *self, void *item_a, void *item_b) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  if (self->compare_fn) {
    return (*self->compare_fn)(item_a, item_b);
  } else {
    return strcmp(item_a, item_b);
  }
}

size_t prom_linked_list_size(prom_linked_list_t *self) {
  PROM_ASSERT(self != NULL);
  return self->size;
}

int prom_linked_list_set_free_fn(prom_linked_list_t *self, prom_linked_list_free_item_fn free_fn) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  self->free_fn = free_fn;
  return 0;
}

int prom_linked_list_set_compare_fn(prom_linked_list_t *self, prom_linked_list_compare_item_fn compare_fn) {
  PROM_ASSERT(self != NULL);
  if (self == NULL) return 1;
  self->compare_fn = compare_fn;
  return 0;
}

void prom_linked_list_no_op_free(void *item) {}

#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "libhash.h"
#include "prime.h"
#include "strdup/strdup.h"

static const int H_DEFAULT_CAPACITY = 50;

/**
 * Resize the hash set. This implementation has a set capacity;
 * hash collisions rise beyond the capacity and `hs_insert` will fail.
 * To mitigate this, we resize up if the load (measured as the ratio of keys
 * count to capacity) is less than .1, or down if the load exceeds .7. To
 * resize, we create a new set approx. 1/2x or 2x times the current set
 * size, then insert into it all non-deleted keys.
 *
 * @param hs
 * @param base_capacity
 * @return int
 */
static void hs_resize(hash_set *hs, const int base_capacity) {
  if (base_capacity < 0) {
    return;
  }

  hash_set *new_hs = hs_init(base_capacity);

  for (int i = 0; i < hs->capacity; i++) {
    const char *r = hs->keys[i];

    if (r != NULL) {
      hs_insert(new_hs, r);
    }
  }

  hs->base_capacity = new_hs->base_capacity;
  hs->count = new_hs->count;

  const int tmp_capacity = hs->capacity;

  hs->capacity = new_hs->capacity;
  new_hs->capacity = tmp_capacity;

  char **tmp_keys = hs->keys;
  hs->keys = new_hs->keys;
  new_hs->keys = tmp_keys;

  hs_delete_set(new_hs);
}

/**
 * Resize the set to a larger size, the first prime subsequent
 * to approx. 2x the base capacity.
 *
 * @param hs
 */
static void hs_resize_up(hash_set *hs) {
  const int new_capacity = hs->base_capacity * 2;

  hs_resize(hs, new_capacity);
}

/**
 * Resize the set to a smaller size, the first prime subsequent
 * to approx. 1/2x the base capacity.
 *
 * @param hs
 */
static void hs_resize_down(hash_set *hs) {
  const int new_capacity = hs->base_capacity / 2;

  hs_resize(hs, new_capacity);
}

/**
 * Delete a key and deallocate its memory
 *
 * @param r key to delete
 */
static void hs_delete_key(char *r) { free(r); }

hash_set *hs_init(int base_capacity) {
  if (!base_capacity) {
    base_capacity = H_DEFAULT_CAPACITY;
  }

  hash_set *hs = malloc(sizeof(hash_set));
  hs->base_capacity = base_capacity;

  hs->capacity = next_prime(hs->base_capacity);
  hs->count = 0;
  hs->keys = calloc((size_t)hs->capacity, sizeof(char *));

  return hs;
}

void hs_insert(hash_set *hs, const void *key) {
  if (hs == NULL) {
    return;
  }

  const int load = hs->count * 100 / hs->capacity;
  if (load > 70) {
    hs_resize_up(hs);
  }

  void *new_record = strdup(key);

  int idx = h_resolve_hash(key, hs->capacity, 0);
  char *current_key = hs->keys[idx];
  int i = 1;

  // i.e. if there was a collision
  while (current_key != NULL) {
    // already exists
    if (strcmp(current_key, key) == 0) {
      return;
    }

    // TODO: verify i is 1..
    idx = h_resolve_hash(new_record, hs->capacity, i);
    current_key = hs->keys[idx];
    i++;
  }

  hs->keys[idx] = new_record;
  hs->count++;
}

int hs_contains(hash_set *hs, const char *key) {
  int idx = h_resolve_hash(key, hs->capacity, 0);
  char *current_key = hs->keys[idx];

  int i = 1;
  while (current_key != NULL) {
    if (strcmp(current_key, key) == 0) {
      return 1;
    }

    idx = h_resolve_hash(key, hs->capacity, i);
    current_key = hs->keys[idx];
    i++;
  }

  return 0;
}

void hs_delete_set(hash_set *hs) {
  for (int i = 0; i < hs->capacity; i++) {
    char *r = hs->keys[i];

    if (r != NULL) {
      hs_delete_key(r);
    }
  }

  free(hs->keys);
  free(hs);
}

int hs_delete(hash_set *hs, const char *key) {
  const int load = hs->count * 100 / hs->capacity;

  if (load < 10) {
    hs_resize_down(hs);
  }

  int i = 0;
  int idx = h_resolve_hash(key, hs->capacity, i);

  char *current_key = hs->keys[idx];

  while (current_key != NULL) {
    if (strcmp(current_key, key) == 0) {
      hs_delete_key(current_key);
      hs->keys[idx] = NULL;

      hs->count--;

      return 1;
    }

    idx = h_resolve_hash(key, hs->capacity, ++i);
    current_key = hs->keys[idx];
  }

  return 0;
}

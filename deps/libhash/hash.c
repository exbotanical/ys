#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "libhash.h"

#define _DEFAULT_SOURCE /* For strdup w/ c99 */

static h_record H_RECORD_SENTINEL = {NULL, NULL};

static const int H_DEFAULT_CAPACITY = 50;
static const int H_PRIME_1 = 151;
static const int H_PRIME_2 = 163;

static int is_prime(const int x);
static int next_prime(int x);

/**
 * @brief Hash a given ASCII key, where `prime` is a prime number
 * larger than 128 (ASCII alphabet max)
 *
 * @param key
 * @param prime
 * @param capacity
 * @return int
 */
static int h_hash(const char *key, const int prime, const int capacity) {
  long hash = 0;

  const int len_s = strlen(key);
  for (int i = 0; i < len_s; i++) {
    // convert the key to a large integer
    hash += (long)pow(prime, len_s - (i + 1)) * key[i];
    // reduce said large integer to a fixed range
    hash = hash % capacity;
  }

  return (int)hash;
}

/**
 * @brief Resolve a hash from the given key, using open addressed
 * double-hashing. This method is adjusted contingent on the number of attempts
 * to resolve a hash without a collision. If no collisions have occurred, i == 0
 * and we resolve to `hash_a`. If a collision occurs, we modify the hash with
 * `hash_b`. Finally, if `hash_b` returns 0, the second term is reduced to 0,
 * causing the table to attempt inserting into the same index indefinitely. We
 * mitigate this behavior by adding 1 to the result of `hash_b`, ensuring it is
 * never 0.
 *
 * @param key
 * @param capacity
 * @param attempt Number of attempts made to generate a non-colliding hash.
 * @return int
 */
static int h_resolve_hash(const char *key, const int capacity,
                          const int attempt) {
  const int hash_a = h_hash(key, H_PRIME_1, capacity);
  const int hash_b = h_hash(key, H_PRIME_2, capacity);

  return (hash_a + (attempt * (hash_b + 1))) % capacity;
}

/**
 * @brief Resize the hash table. This implementation has a set capacity;
 * hash collisions rise beyond the capacity and `h_instert` will fail.
 * To mitigate this, we resize up if the load (measured as the ratio of records
 * count to capacity) is less than .1, or down if the load exceeds .7. To
 * resize, we create a new table approx. 1/2x or 2x times the current table
 * size, then insert into it all non-deleted records.
 *
 * @param ht
 * @param base_capacity
 * @return int
 */
static void h_resize(h_table *ht, const int base_capacity) {
  if (base_capacity < 0) {
    return;
  }

  h_table *new_ht = h_init_table(base_capacity);

  for (int i = 0; i < ht->capacity; i++) {
    h_record *r = ht->records[i];

    if (r != NULL && r != &H_RECORD_SENTINEL) {
      h_insert(new_ht, r->key, r->value);
    }
  }

  ht->base_capacity = new_ht->base_capacity;
  ht->count = new_ht->count;

  const int tmp_capacity = ht->capacity;

  ht->capacity = new_ht->capacity;
  new_ht->capacity = tmp_capacity;

  h_record **tmp_records = ht->records;
  ht->records = new_ht->records;
  new_ht->records = tmp_records;

  h_delete_table(new_ht);
}

/**
 * @brief Resize the table to a larger size, the first prime subsequent
 * to approx. 2x the base capacity.
 *
 * @param ht
 */
static void h_resize_up(h_table *ht) {
  const int new_capacity = ht->base_capacity * 2;

  h_resize(ht, new_capacity);
}

/**
 * @brief Resize the table to a smaller size, the first prime subsequent
 * to approx. 1/2x the base capacity.
 *
 * @param ht
 */
static void h_resize_down(h_table *ht) {
  const int new_capacity = ht->base_capacity / 2;

  h_resize(ht, new_capacity);
}

/**
 * @brief Initialize a new hash table record with the given k, v pair
 *
 * @param k Record key
 * @param v Record value
 * @return h_record*
 */
static h_record *h_init_record(const char *k, void *v) {
  h_record *r = malloc(sizeof(h_record));
  r->key = strdup(k);
  r->value = v;

  return r;
}

/**
 * @brief Delete a record and deallocate its memory
 *
 * @param r Record to delete
 */
static void h_delete_record(h_record *r) {
  free(r->key);
  // free(r->value);
  free(r);
}

/**
 * @brief Initialize a new hash table with a size of `max_size`
 *
 * @param max_size The hash table capacity
 * @return h_table*
 */
h_table *h_init_table(int base_capacity) {
  if (!base_capacity) {
    base_capacity = H_DEFAULT_CAPACITY;
  }

  h_table *ht = malloc(sizeof(h_table));
  ht->base_capacity = base_capacity;

  ht->capacity = next_prime(ht->base_capacity);
  ht->count = 0;
  ht->records = calloc((size_t)ht->capacity, sizeof(h_record *));

  return ht;
}

/**
 * @brief Insert a key, value pair into the given hash table.
 *
 * @param ht
 * @param key
 * @param value
 */
void h_insert(h_table *ht, const char *key, void *value) {
  if (ht == NULL) {
    return;
  }

  const int load = ht->count * 100 / ht->capacity;
  if (load > 70) {
    h_resize_up(ht);
  }

  h_record *new_record = h_init_record(key, value);

  int idx = h_resolve_hash(new_record->key, ht->capacity, 0);
  h_record *current_record = ht->records[idx];
  int i = 1;

  // i.e. if there was a collision
  while (current_record != NULL && current_record != &H_RECORD_SENTINEL) {
    // update existing key/value
    if (strcmp(current_record->key, key) == 0) {
      h_delete_record(current_record);
      ht->records[idx] = new_record;

      return;
    }

    // TODO verify i is 1..
    idx = h_resolve_hash(new_record->key, ht->capacity, i);
    current_record = ht->records[idx];
    i++;
  }

  ht->records[idx] = new_record;
  ht->count++;
}

/**
 * @brief Search for the record corresponding to the given key
 *
 * @param ht
 * @param key
 * @return char*
 */
h_record *h_search(h_table *ht, const char *key) {
  int idx = h_resolve_hash(key, ht->capacity, 0);
  h_record *current_record = ht->records[idx];
  int i = 1;

  while (current_record != NULL && current_record != &H_RECORD_SENTINEL) {
    if (strcmp(current_record->key, key) == 0) {
      return current_record;
    }

    idx = h_resolve_hash(key, ht->capacity, i);
    current_record = ht->records[idx];
    i++;
  }

  return NULL;
}

char *h_retrieve(h_table *ht, const char *key) {
  h_record *r = h_search(ht, key);

  return r ? r->value : NULL;
}

/**
 * @brief Delete a hash table and deallocate its memory
 *
 * @param ht Hash table to delete
 */
void h_delete_table(h_table *ht) {
  for (int i = 0; i < ht->capacity; i++) {
    h_record *r = ht->records[i];

    if (r != NULL && r != &H_RECORD_SENTINEL) {
      h_delete_record(r);
    }
  }

  free(ht->records);
  free(ht);
}

/**
 * @brief Delete a record for the given key `key`. Because records
 * may be part of a collision chain, and removing them completely
 * could cause infinite lookup attempts, we replace the deleted record
 * with a NULL sentinel record.
 *
 * @param ht
 * @param key
 *
 * @return int 0 if a record was deleted, 1 if no record corresponding
 * to the given key could be found
 */
int h_delete(h_table *ht, const char *key) {
  const int load = ht->count * 100 / ht->capacity;

  if (load < 10) {
    h_resize_down(ht);
  }

  int ret = 1;
  int i = 0;
  int idx = h_resolve_hash(key, ht->capacity, i);

  h_record *current_record = ht->records[idx];

  while (current_record != NULL && current_record != &H_RECORD_SENTINEL) {
    if (strcmp(current_record->key, key) == 0) {
      h_delete_record(current_record);
      ht->records[idx] = &H_RECORD_SENTINEL;

      ret = 0;
      ht->count--;

      return ret;
    }

    idx = h_resolve_hash(key, ht->capacity, ++i);
    current_record = ht->records[idx];
  }

  return ret;
}

/**
 * @brief Determine whether the given integer `x` is prime.
 *
 * @param x
 * @return int
 */
static int is_prime(const int x) {
  if (x < 2 || (x % 2) == 0) {
    return false;
  }

  if (x < 4) {
    return true;
  }

  for (int i = 3; i <= floor(sqrt((double)x)); i += 2) {
    if ((x % i) == 0) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Return the next prime after the given integer `x`,
 * or `x` if `x` is prime. Successive, brute-force resolution.
 *
 * @param x
 * @return int
 */
static int next_prime(int x) {
  while (!is_prime(x)) {
    x++;
  }

  return x;
}

#ifndef LIBHASH_H
#define LIBHASH_H

/**
 * A hash table record i.e. key / value pair
 */
typedef struct {
  char *key;
  void *value;
} ht_record;

/**
 * A hash table
 */
typedef struct {
  /**
   * Max number of records which may be stored in the hash table. Adjustable.
   * Calculated as the first prime subsequent to the base capacity.
   */
  int capacity;

  /**
   * Base capacity (used to calculate load for resizing)
   */
  int base_capacity;

  /**
   * Number of non-NULL records in the hash table
   */
  int count;

  /**
   * The hash table's records
   */
  ht_record **records;
} hash_table;

/**
 * Initialize a new hash table with a size of `max_size`
 *
 * @param max_size The hash table capacity
 * @return hash_table*
 */
hash_table *ht_init(int base_capacity);

/**
 * Insert a key, value pair into the given hash table.
 *
 * @param ht
 * @param key
 * @param value
 */
void ht_insert(hash_table *ht, const char *key, void *value);

/**
 * Search for the record corresponding to the given key
 *
 * @param ht
 * @param key
 * @return char*
 */
ht_record *ht_search(hash_table *ht, const char *key);

char *ht_get(hash_table *ht, const char *key);

/**
 * Delete a hash table and deallocate its memory
 *
 * @param ht Hash table to delete
 */
void ht_delete_table(hash_table *ht);

/**
 * Delete a record for the given key `key`. Because records
 * may be part of a collision chain, and removing them completely
 * could cause infinite lookup attempts, we replace the deleted record
 * with a NULL sentinel record.
 *
 * @param ht
 * @param key
 *
 * @return int 1 if a record was deleted, 0 if no record corresponding
 * to the given key could be found
 */
int ht_delete(hash_table *ht, const char *key);

typedef struct {
  /**
   * Max number of keys which may be stored in the hash set. Adjustable.
   * Calculated as the first prime subsequent to the base capacity.
   */
  int capacity;

  /**
   * Base capacity (used to calculate load for resizing)
   */
  int base_capacity;

  /**
   * Number of non-NULL keys in the hash set
   */
  int count;

  /**
   * The hash set's keys
   */
  char **keys;
} hash_set;

/**
 * Initialize a new hash set with a size of `max_size`
 *
 * @param max_size The hash set capacity
 * @return hash_set*
 */
hash_set *hs_init(int base_capacity);

/**
 * Insert a key into the given hash set.
 *
 * @param hs
 * @param key
 */
void hs_insert(hash_set *hs, const void *key);

/**
 * Check whether the given hash set contains a key `key`
 *
 * @param hs
 * @param key
 * @return int 1 for true, 0 for false
 */
int hs_contains(hash_set *hs, const char *key);

/**
 * Delete a hash set and deallocate its memory
 *
 * @param hs Hash set to delete
 */
void hs_delete_set(hash_set *hs);

/**
 * Delete the given key `key`.
 *
 * @param hs
 * @param key
 *
 * @return int 1 if a key was deleted, 0 if the given key did not already exist
 * in the set
 */
int hs_delete(hash_set *hs, const char *key);

#endif /* LIBHASH_H */

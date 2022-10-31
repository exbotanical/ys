#ifndef HASH_H
#define HASH_H

/**
 * A hash table record i.e. key / value pair
 */
typedef struct {
	char* key;
	char* value;
} h_record;

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
	h_record** records;
} h_table;

h_table* h_init_table (int base_capacity);

void h_insert (h_table* ht, const char* key, const char* value);

h_record* h_search (h_table* ht, const char* key);

char* h_retrieve (h_table* ht, const char* key);

void h_delete_table (h_table* ht);

int h_delete (h_table* ht, const char* key);

#endif /* HASH_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the maximum size of the hash table
#define HASH_TABLE_SIZE 1000

// Define the linked list node structure
typedef struct node {
  char* key;
  struct node* next;
} node_t;

// Define the hash table structure
typedef struct {
  node_t* buckets[HASH_TABLE_SIZE];
} hash_table_t;

// Define the hash function
unsigned int hash(char* key) {
  unsigned int hash = 0;
  int c;

  while ((c = *key++)) {
    hash = hash * 33 + c;
  }

  return hash;
}

// Define the function to add a key-value pair to the hash table
void add_key_value(hash_table_t* hash_table, char* key) {
  unsigned int bucket_index = hash(key) % HASH_TABLE_SIZE;

  node_t* current = hash_table->buckets[bucket_index];
  while (current) {
    if (strcmp(current->key, key) == 0) {
      return;  // key already exists in hash table
    }
    current = current->next;
  }

  // key does not exist in hash table, add it to the front of the linked list
  node_t* new_node = malloc(sizeof(node_t));
  new_node->key = key;
  new_node->next = hash_table->buckets[bucket_index];
  hash_table->buckets[bucket_index] = new_node;
}

// Define the function to check if a key exists in the hash table
int has_key(hash_table_t* hash_table, char* key) {
  unsigned int bucket_index = hash(key) % HASH_TABLE_SIZE;

  node_t* current = hash_table->buckets[bucket_index];
  while (current) {
    if (strcmp(current->key, key) == 0) {
      return 1;  // key exists in hash table
    }
    current = current->next;
  }

  return 0;  // key does not exist in hash table
}

// Example usage
int main() {
  // Initialize the hash table
  hash_table_t hash_table;
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    hash_table.buckets[i] = NULL;
  }

  // Add keys to the hash table
  add_key_value(&hash_table, "a");
  add_key_value(&hash_table, "b");
  add_key_value(&hash_table, "c");

  // Check if a key exists in the hash table
  if (has_key(&hash_table, "b")) {
    printf("Key exists in hash table\n");
  } else {
    printf("Key does not exist in hash table\n");
  }

  return 0;
}
// TODO: use this

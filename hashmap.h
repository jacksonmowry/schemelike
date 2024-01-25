#pragma once
#include "parse.h"
#include <stdbool.h>
#include <stdint.h>

#define TOMBSTONE UINT64_MAX
#define STR_BUF_LEN 64

bool is_prime(int);
int next_prime(int);

typedef struct {
  char *key;
  ast_node value;
} pair;

/* Pretty Printing Functions, char[STR_BUF_LEN] max output */
/* Can be used to define a custom print function for more complex datatypes */
typedef void (*print_function)(char *, void *);
void unsigned_print(char *, void *);
void signed_print(char *, void *);
void double_print(char *, void *);
void address_print(char *, void *);
void string_print(char *, void *);

// Hashing Functions
typedef uint64_t (*hash_function)(void *);
/* uint64_t no_hash(void *); */
/* uint64_t basic_hash(void *); */
/* uint64_t fnv_hash(void *); */
uint64_t fnv_string_hash(void *);

// Equality Functions
typedef bool (*equals_function)(void *, void *);
bool value_equals(void *, void *);
bool str_equals(void *, void *);

// Hashmap
typedef struct {
  float load_factor;
  int capacity;
  int size;
  int collisions;
  hash_function hash_func;
  equals_function equals_func;
  pair *array;
} hashmap;

hashmap hashmap_init(hash_function, equals_function, float, int);
void hashmap_free(hashmap *);
pair *hashmap_find(hashmap *, char *);
pair *hashmap_first_avail(hashmap *, char *, int *);
void hashmap_resize(hashmap *, int);
void hashmap_insert(hashmap *, char *, ast_node);
ast_node hashmap_get(hashmap *, char *);
ast_node hashmap_delete(hashmap *, char *);
void hashmap_print(hashmap *, print_function, print_function);
hashmap hashmap_copy(hashmap *);

#ifndef PPM_TRIE_H_
#define PPM_TRIE_H_

#include <stdlib.h>

#include "data.h"

#define PTR(node) (trie->list->data + node)

// Scaffold type definitions
struct __node_struct__;
struct __trie_struct__;
struct __nodelist_struct__;
struct __fraction_struct__;
typedef struct __node_struct__ node_t;
typedef struct __trie_struct__ trie_t;
typedef struct __nodelist_struct__ nodelist_t;
typedef struct __fraction_struct__ fraction_t;

typedef int node_i;

// Actual type definitions
struct __node_struct__ {
    symbol_t symbol;
    unsigned int count;
    node_i down;  // points to the leftmost child of the node
    node_i right; // points to the next sibling of the node
    node_i vine;
};

struct __nodelist_struct__ {
    node_t * data;
    node_i ptr;
    size_t size;
};

struct __trie_struct__ {
    unsigned int order;
    node_i root;
    node_i base_ptr;
    nodelist_t * list;
};

struct __fraction_struct__ {
    unsigned int numerator;
    unsigned int denominator;
};

// Initializer and destroyer functions
trie_t * trie_create(unsigned int order);
void trie_destroy(trie_t * trie);

// State functions
void trie_print(trie_t * trie);
int trie_dump(trie_t * trie, char * filename);
int trie_load(trie_t * trie, char * filename);

// Updater function
fraction_t trie_add(trie_t * trie, symbol_t symbol);

// Internal functions
void __trie_print_node(trie_t * trie, node_i node, unsigned int level);
node_i __node_getadd_symbol_child(trie_t * trie, node_i node, symbol_t symbol);
unsigned int __trie_get_node_level(trie_t * trie, node_i node);

nodelist_t * __nodelist_create();
void __nodelist_extend(nodelist_t * nodelist);
void __nodelist_free(nodelist_t * nodelist);
node_i __node_malloc(nodelist_t * nodelist);


#endif // PPM_TRIE_H_

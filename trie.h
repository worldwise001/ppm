#ifndef PPM_TRIE_H_
#define PPM_TRIE_H_

#include "data.h"
#include "node.h"
#include "nodelist.h"

// Scaffold type definitions
struct __trie_struct__;
struct __context_struct__;
struct __context_entry_struct__;
typedef struct __trie_struct__ trie_t;
typedef struct __context_struct__ context_t;
typedef struct __context_entry_struct__ context_entry_t;

// Actual type definitions

struct __context_entry_struct__ {
    symbol_t symbol;                  // symbol history
    fraction_t probability;           // probability of symbol to be encoded
    unsigned char escape:1;           // bitfield that shows whether an escape was thrown in this context
};

struct __context_struct__ {
    context_entry_t * entries;        // array context entries
    unsigned long * escape_counts;     // escape counts for each context
    fraction_t avg_escape_count;      // average escape count per character
};

struct __trie_struct__ {
    unsigned int order;               // maintains the order of the trie
    node_i root;                      // pointer to the root
    node_i base_ptr;                  // pointer to the base
    nodelist_t * list;                // contiguously malloced blob of nodes
    context_t context;                // context of the trie
    fraction_t character_probability; // probability of a single character occurring
};

// Initializer and destroyer functions
trie_t * trie_create(unsigned int order);
void trie_destroy(trie_t * trie);

// State functions
void trie_print(trie_t * trie);
void trie_print_state(trie_t * trie);
int trie_dump(trie_t * trie, char * filename);
int trie_load(trie_t * trie, char * filename);
void trie_clear_context(trie_t * trie);
void trie_clear_escapes(trie_t * trie);

// Updater function
void trie_add(trie_t * trie, symbol_t symbol);

// Informative functions
fraction_t trie_get_probability_encoding(trie_t * trie);
double trie_get_bit_encoding(trie_t * trie);

// Internal trie functions
void __trie_print_node(trie_t * trie, node_i node, unsigned int level);
node_i __trie_getadd_symbol_child(trie_t * trie, node_i node, symbol_t symbol);
unsigned int __trie_get_node_level(trie_t * trie, node_i node);
void __trie_update_context(trie_t * trie, symbol_t symbol);
void __trie_update_probabilities(trie_t * trie, node_i parent, symbol_t symbol);

#endif // PPM_TRIE_H_

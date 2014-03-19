#ifndef PPM_TRIE_H_
#define PPM_TRIE_H_

// Scaffold type definitions
struct __node_struct__;
struct __trie_struct__;
typedef struct __node_struct__ node_t;
typedef struct __trie_struct__ trie_t;

// symbol_t is a typedef depending on how many bytes we consider to be a "symbol"
typedef char symbol_t;

// Actual type definitions
struct __node_struct__ {
    symbol_t symbol;
    unsigned int count;
    node_t * down; // points to the leftmost child of the node
    node_t * right; // points to the next sibling of the node
    node_t * vine;
};

struct __trie_struct__ {
    node_t * root;
    unsigned int order;
    node_t * base_ptr;
};

// Initializer and destroyer functions
trie_t * trie_create(unsigned int order);
void trie_destroy(trie_t * trie);

// State functions
void trie_print(trie_t * trie);
int trie_dump(trie_t * trie, char * filename);
int trie_load(trie_t * trie, char * filename);

// Updater function
void trie_add(trie_t * trie, symbol_t symbol);

// Internal functions
void __trie_destroy_node(node_t * node);
void __trie_print_node(node_t * node, unsigned int level);
node_t * __node_getadd_symbol_child(node_t * node, symbol_t symbol);
unsigned int __trie_get_node_level(trie_t * trie, node_t * node);

#endif // PPM_TRIE_H_

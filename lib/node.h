#ifndef PPM_NODE_H_
#define PPM_NODE_H_

#include "data.h"

#define PTR(node) (root + node)

// Scaffold type definitions
struct __node_struct__;
typedef struct __node_struct__ node_t;

typedef unsigned int node_i;

// Actual type definitions
struct __node_struct__ {
    symbol_t symbol;
    unsigned long count;
    node_i down;                      // points to the leftmost child of the node
    node_i right;                     // points to the next sibling of the node
    node_i vine;                      // points to the upper-order character node
    node_i parent;                    // points to the parent node
    unsigned long child_escape_count; // escape count
};

// Internal node functions
unsigned long __node_get_single_child_count(node_t * root, node_i node, symbol_t symbol);
unsigned long __node_get_total_child_count(node_t * root, node_i node);
fraction_t __node_get_symbol_probability(node_t * root, node_i node, symbol_t symbol);
fraction_t __node_get_escape_probability(node_t * root, node_i node);

#endif // PPM_NODE_H_

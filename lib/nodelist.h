#ifndef PPM_NODELIST_H_
#define PPM_NODELIST_H_

#include "data.h"
#include "node.h"

#include <stdlib.h>

#define NODE_SIZE 50000

// Scaffold type definitions
struct __nodelist_struct__;
typedef struct __nodelist_struct__ nodelist_t;

struct __nodelist_struct__ {
    node_t * data;
    node_i ptr;
    size_t size;
};

// Internal nodelist functions
nodelist_t * __nodelist_create(void);
void __nodelist_extend(nodelist_t * nodelist);
void __nodelist_free(nodelist_t * nodelist);
node_i __node_malloc(nodelist_t * nodelist);
nodelist_t * __nodelist_duplicate(nodelist_t * nodelist);

#endif // PPM_NODELIST_H_

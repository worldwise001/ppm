#include "nodelist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// black magic follows here
nodelist_t * __nodelist_create() {
    nodelist_t * nodelist;
    nodelist = malloc(sizeof(nodelist_t));
    memset(nodelist, 0, sizeof(nodelist_t));
    nodelist->data = malloc(sizeof(node_t)*NODE_SIZE);
    nodelist->ptr = 1;
    nodelist->size = NODE_SIZE;
    return nodelist;
}

void __nodelist_extend(nodelist_t * nodelist) {
    node_t * data, *odata;

    if ( nodelist->ptr >= nodelist->size ) {
        odata = nodelist->data;
        nodelist->data = NULL;

        data = realloc(odata, sizeof(node_t)*(nodelist->size+NODE_SIZE));
        if (data == NULL) {
            printf("Error expanding node tree\n");
            exit(1);
        }
        nodelist->data = data;
        nodelist->size += NODE_SIZE;
    }
}

void __nodelist_free(nodelist_t * nodelist) {
    free(nodelist->data);
    free(nodelist);
}

node_i __node_malloc(nodelist_t * nodelist) {
    node_i node;
    node_t * nodeptr;
    __nodelist_extend(nodelist);
    node = nodelist->ptr;
    nodelist->ptr++;
    nodeptr = nodelist->data + node;
    memset(nodeptr, 0, sizeof(node_t));
    return node;
}

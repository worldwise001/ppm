#include "trie.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Initializer and destroyer functions

trie_t * trie_create(unsigned int order) {
    // Allocate memory for the actual trie
    trie_t * trie = malloc(sizeof(trie_t));
    memset(trie, 0, sizeof(trie_t));

    // Initialize the nodelist
    trie->list = __nodelist_create();    

    // Initialize the root node
    trie->root = __node_malloc(trie->list);

    // Set the base_ptr to root
    trie->base_ptr = trie->root;

    // Set the order
    trie->order = order;

    // Return the trie
    return trie;
}

void trie_destroy(trie_t * trie) {
    __nodelist_free(trie->list);
    free(trie);
}

// State functions
void trie_print(trie_t * trie) {
    __trie_print_node(trie, trie->root, 0);
}

int trie_dump(trie_t * trie, char * filename) {
    return 0;
}

int trie_load(trie_t * trie, char * filename) {
//    int fd;
//    nodelist_t * nl;

    return 0;
}

// Updater function
fraction_t trie_add(trie_t * trie, symbol_t symbol) {
    node_i x, y, z, s, a, b, f;
    fraction_t fraction;

    // step 0
    x = trie->base_ptr;                              // follow the base pointer to node x

    // step 1
    if (__trie_get_node_level(trie, x) < trie->order) {
        // if the base pointer is not at the height of the tree, then we follow a different set of rules
        // this only applies in the early trie initialization stages

        y = x;                                       // assignment from x to y for readability
        // add s as a new child of y, meaning we can grow the tree
        s = __node_getadd_symbol_child(trie, y, symbol);
        if (trie->base_ptr == trie->root) {
            PTR(s)->vine = trie->root;
        }
        trie->base_ptr = s;                          // set the base to point to s
        a = s;                                       
    } else {
        // if the base pointer IS at the height of the tree, then we don't actually want to be growing the trie at all
        // so we follow the vine pointer from the base pointer to start appending

        y = PTR(x)->vine;                                 // follow the vine pointer from x to y
        // add s as a new child node of y OR increment the count by 1
        s = __node_getadd_symbol_child(trie, y, symbol);
        trie->base_ptr = s;                          // set the base to point to s
        a = s;
    }
    f = y;

    // step 2
    while (PTR(y)->vine != 0) {                        // repeat while y has a vine
        z = PTR(y)->vine;                                 // follow the vine pointer from y to z
        // add s as a new child node of z OR increment the count by 1
        s = __node_getadd_symbol_child(trie, z, symbol);
        b = s;
        PTR(a)->vine = b;
        a = b;                                       // set a to b (shift context)
        y = z;                                       // set y to z (shift context)
    }

    if (PTR(a)->vine == 0) {
        PTR(a)->vine = trie->root;                        // add a vine back to root
    }

    f = PTR(f)->down;
    fraction.numerator = PTR(trie->base_ptr)->count;
    for (fraction.denominator = 0; f != 0; fraction.denominator += PTR(f)->count, f = PTR(f)->right);
    return fraction;
}

void __trie_print_node(trie_t * trie, node_i node, unsigned int level) {
    if (node == 0) {
        printf("\n");
        return;
    }
    printf("%s-- (%c,%03d) ", (level==0?"  ":"|"), PTR(node)->symbol, PTR(node)->count);
    __trie_print_node(trie, PTR(node)->down, level+1);
    for (unsigned int i = 0; i < level; i++) {
        printf("%12s", " ");
    }
    __trie_print_node(trie, PTR(node)->right, level);
}

node_i __node_getadd_symbol_child(trie_t * trie, node_i node, symbol_t symbol) {
    node_i child, tmp;

    child = PTR(node)->down;
    if (child == 0) {
        // if there are no children nodes, then create one
        child = __node_malloc(trie->list);
        PTR(node)->down = child;
        PTR(child)->symbol = symbol;
        PTR(child)->count = 1;
        return child;
    }
    if (PTR(child)->symbol == symbol) {
        // if the first child node is in fact the one we want, return it
        PTR(child)->count++;
        return child;
    }
    for (; PTR(child)->right != 0; child = PTR(child)->right) {
        // search for the child node we want and return it
        if (PTR(PTR(child)->right)->symbol == symbol) {
            PTR(PTR(child)->right)->count++;
            return PTR(child)->right;
        }
    }
    // if there are children nodes but none of them are the ones we want, create one
    tmp = __node_malloc(trie->list);
    PTR(tmp)->symbol = symbol;
    PTR(tmp)->count = 1;
    PTR(child)->right = tmp;
    return tmp;
}

unsigned int __trie_get_node_level(trie_t * trie, node_i node) {
    unsigned int count = 0;
    if (PTR(node)->vine == 0) {
        return 0;
    }
    for (; trie->root != node; node = PTR(node)->vine, count++);
    return count;
}

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

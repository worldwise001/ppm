#include "trie.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Initializer and destroyer functions

trie_t * trie_create(unsigned int order) {
    // Allocate memory for the actual trie
    trie_t * trie = malloc(sizeof(trie_t));
    memset(trie, 0, sizeof(trie_t));
    
    // Allocate memory for the root
    trie->root = malloc(sizeof(node_t));
    memset(trie->root, 0, sizeof(node_t));
    trie->base_ptr = trie->root;

    // Set the order
    trie->order = order;

    // Return the trie
    return trie;
}

void trie_destroy(trie_t * trie) {
    __trie_destroy_node(trie->root);
    free(trie);
}

// State functions
void trie_print(trie_t * trie) {
    __trie_print_node(trie->root, 0);
}

int trie_dump(trie_t * trie, char * filename) {
    printf("Not implemented\n");
    return 1;
}

int trie_load(trie_t * trie, char * filename) {
    printf("Not implemented\n");
    return 1;
}

// Updater function
void trie_add(trie_t * trie, symbol_t symbol) {
    node_t *x, *y, *z, *s, *a, *b;

    // step 0
    x = trie->base_ptr;                              // follow the base pointer to node x

    // step 1
    if (__trie_get_node_level(trie, x) < trie->order) {
        // if the base pointer is not at the height of the tree, then we follow a different set of rules
        // this only applies in the early trie initialization stages

        y = x;                                       // assignment from x to y for readability
        s = __node_getadd_symbol_child(y, symbol);   // add s as a new child of y, meaning we can grow the tree
        if (trie->base_ptr == trie->root) {
            s->vine = trie->root;
        }
        trie->base_ptr = s;                          // set the base to point to s
        a = s;                                       // yeah let's just follow the paper assigment
    } else {
        // if the base pointer IS at the height of the tree, then we don't actually want to be growing the trie at all
        // so we follow the vine pointer from the base pointer to start appending

        y = x->vine;                                 // follow the vine pointer from x to y
        s = __node_getadd_symbol_child(y, symbol);   // add s as a new child node of y OR increment the count by 1
        trie->base_ptr = s;                          // set the base to point to s
        a = s;
    }

    // step 2
    while (y->vine != NULL) {                        // repeat while y has a vine
        z = y->vine;                                 // follow the vine pointer from y to z
        s = __node_getadd_symbol_child(z, symbol);   // add s as a new child node of z OR increment the count by 1
        b = s;
        a->vine = b;
        a = b;                                       // set a to b (shift context)
        y = z;                                       // set y to z (shift context)
    }

    if (a->vine == NULL) {
        a->vine = trie->root;                        // add a vine back to root
    }
}

// Internal functions
void __trie_destroy_node(node_t * node) {
    if (node == NULL)
        return;
    __trie_destroy_node(node->down);
    __trie_destroy_node(node->right);
    free(node);
}

void __trie_print_node(node_t * node, unsigned int level) {
    if (node == NULL) {
        printf("\n");
        return;
    }
    printf("%s-- (%c,%03d) ", (level==0?"  ":"|"), node->symbol, node->count);
    __trie_print_node(node->down, level+1);
    for (unsigned int i = 0; i < level; i++) {
        printf("%12s", " ");
    }
    __trie_print_node(node->right, level);
}

node_t * __node_getadd_symbol_child(node_t * node, symbol_t symbol) {
    node_t * child = node->down;
    if (child == NULL) {
        // if there are no children nodes, then create one
        child = malloc(sizeof(node_t));
        memset(child, 0, sizeof(node_t));
        node->down = child;
        child->symbol = symbol;
        child->count = 1;
        return child;
    }
    if (child->symbol == symbol) {
        // if the first child node is in fact the one we want, return it
        child->count++;
        return child;
    }
    for (; child->right != NULL; child = child->right) {
        // search for the child node we want and return it
        if (child->right->symbol == symbol) {
            child->right->count++;
            return child->right;
        }
    }
    // if there are children nodes but none of them are the ones we want, create one
    child->right = malloc(sizeof(node_t));
    memset(child->right, 0, sizeof(node_t));
    child->right->symbol = symbol;
    child->right->count = 1;
    return child->right;
}

unsigned int __trie_get_node_level(trie_t * trie, node_t * node) {
    unsigned int count = 0;
    if (node->vine == NULL) {
        return 0;
    }
    for (; trie->root != node; node = node->vine, count++);
    return count;
}

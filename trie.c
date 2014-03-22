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

    // Initialize the context
    trie->context = malloc(sizeof(symbol_t)*order);
    memset(trie->context, 0, sizeof(symbol_t)*order);

    // Return the trie
    return trie;
}

void trie_destroy(trie_t * trie) {
    __nodelist_free(trie->list);
    free(trie->context);
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
void trie_add(trie_t * trie, symbol_t symbol) {
    node_i x, y, z, s, a, b;
    fraction_t p, pr;
    int order;

    // stops valgrind from complaining
    order = -1;
    memset(&p, 0, sizeof(fraction_t));
    memset(&pr, 0, sizeof(fraction_t));

    // step 0
    x = trie->base_ptr;              // follow the base pointer to node x

    // step 1
    if (__trie_get_node_level(trie, x) < trie->order) {
        // if the base pointer is not at the height of the tree, then we follow a different set of rules
        // this only applies in the early trie initialization stages
        y = x;                       // assignment from x to y for readability
    } else {
        // if the base pointer IS at the height of the tree, then we don't actually want to be growing the trie at all
        // so we follow the vine pointer from the base pointer to start appending
        y = PTR(x)->vine;            // follow the vine pointer from x to y
    }

    // add s as a new child node of y OR increment the count by 1
    s = __node_getadd_symbol_child(trie, y, symbol, &p);
    trie->base_ptr = s;              // set the base to point to s
    a = s;

    // if we found it in the high order context, then note the order and probabilities
    if (p.denominator != 0) {
        pr = p;
        order = __trie_get_node_level(trie, x);
    }

    // step 2
    while (PTR(y)->vine != 0) {          // repeat while y has a vine
        z = PTR(y)->vine;                // follow the vine pointer from y to z
        // add s as a new child node of z OR increment the count by 1
        s = __node_getadd_symbol_child(trie, z, symbol, &p);

        // if we have not yet found it in a higher order context,
        // note the order and probabilities
        if (pr.denominator == 0 && p.denominator != 0) {
            pr = p;
            order = __trie_get_node_level(trie, y);
        }
        b = s;

        PTR(a)->vine = b;                // add a vine to the previous node
        a = b;                           // set a to b (shift context)
        y = z;                           // set y to z (shift context)
    }

    if (PTR(a)->vine == 0) {
        PTR(a)->vine = trie->root;       // add a vine back to root
    }

    // if we did not find the symbol at all,
    // we are in order -1, with default symbol probabilities
    if (order == -1) {
        pr.numerator = 1;
        pr.denominator = 255;
    }

    // print what we found
    printf("%c exists in order %d with probability %d/%d\n", symbol, order, pr.numerator, pr.denominator);
}

// prints the trie in a pretty fashion
void __trie_print_node(trie_t * trie, node_i node, unsigned int level) {
    if (node == 0) {
        printf("\n");
        return;
    }
    // black magic here
    printf("%s-- (%c,%03d) ", (level==0?"  ":"|"), PTR(node)->symbol, PTR(node)->count);
    // print all the children nodes on one line
    __trie_print_node(trie, PTR(node)->down, level+1);
    // more black magic here
    for (unsigned int i = 0; i < level; i++) {
        printf("%12s", " ");
    }
    // print all sibling nodes on new lines
    __trie_print_node(trie, PTR(node)->right, level);
}

// finds or creates the relevant symbol in an order n context in the trie
node_i __node_getadd_symbol_child(trie_t * trie, node_i node, symbol_t symbol, fraction_t * probability) {
    node_i child, ni, result;

    // by default initialize the denominator to 0
    probability->denominator = 0;

    child = PTR(node)->down;
    if (child == 0) {
        // if there are no children nodes, then create one
        child = __node_malloc(trie->list);
        PTR(node)->down = child;
        PTR(child)->symbol = symbol;
        PTR(child)->count = 1;
        // new symbol means numerator is set to 0
        probability->numerator = 0;
        return child;
    }

    // initialize the result node id to 0
    result = 0;
    // search among children nodes to find the symbol in this context
    for (ni = child; ni != 0; ni = PTR(ni)->right) {
        // count all the sibling nodes to form the denominator
        probability->denominator += PTR(ni)->count;
        
        // if the symbol was found in this context...
        if (PTR(ni)->symbol == symbol) {
            // set the numerator
            probability->numerator = PTR(ni)->count;
            // THEN increment (setting the probability before incrementing is required for adaptive arithmetic coding)
            PTR(ni)->count++;
            // set the result
            result = ni;
        }
        // coincidentally move the child pointer
        child = ni;
    }

    // if we have a result, return it
    if (result != 0) {
        return result;
    }

    // if not, we need to create a new sibling
    // denominator is set to 0 again to indicate that the symbol was not found in this context
    probability->denominator = 0;
    // create the node and set the fields
    result = __node_malloc(trie->list);
    PTR(result)->symbol = symbol;
    PTR(result)->count = 1;
    // add the sibling
    PTR(child)->right = result;
    // return it
    return result;
}

// determines the order/height of a node in the trie
unsigned int __trie_get_node_level(trie_t * trie, node_i node) {
    unsigned int count = 0;

    // if the vine is not set, technically is undefined
    if (PTR(node)->vine == 0) {
        return 0;
    }

    // traverse the trie and increment the count
    for (; trie->root != node; node = PTR(node)->vine, count++);

    // return the count
    return count;
}


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

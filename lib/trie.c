#include "trie.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

// Initializer and destroyer functions

trie_t * trie_create(unsigned int order) {
    // Allocate memory for the actual trie
    trie_t * trie = malloc(sizeof(trie_t));
    memset(trie, 0, sizeof(trie_t));

    // Initialize the context
    trie->context.entries = malloc(sizeof(context_entry_t) * order);
    memset(trie->context.entries, 0, sizeof(context_entry_t) * order);
    trie->context.escape_counts = malloc(sizeof(unsigned long) * order);
    memset(trie->context.escape_counts, 0, sizeof(unsigned long) * order);

    // Initialize the nodelist
    trie->list = __nodelist_create();    

    // Initialize the root node
    trie->root = __node_malloc(trie->list);

    // Set the base_ptr to root
    trie->base_ptr = trie->root;

    // Set the order
    trie->order = order;

    // Set the default probability
    trie->character_probability.numerator = 1;
    trie->character_probability.denominator = 255;

    // Return the trie
    return trie;
}

void trie_destroy(trie_t * trie) {
    __nodelist_free(trie->list);
    free(trie->context.escape_counts);
    free(trie->context.entries);
    free(trie);
}

trie_t * trie_duplicate(trie_t * trie) {
    trie_t * dup = malloc(sizeof(trie_t));
    memcpy(dup, trie, sizeof(trie_t));

    dup->context.entries = malloc(sizeof(context_entry_t) * (dup->order));
    memcpy(dup->context.entries, trie->context.entries, sizeof(context_entry_t) * (dup->order));
    dup->context.escape_counts = malloc(sizeof(unsigned long) * (dup->order));
    memcpy(dup->context.escape_counts, trie->context.escape_counts, sizeof(unsigned long) * (dup->order));

    dup->list = __nodelist_duplicate(trie->list);

    return dup;
}

// State functions
void trie_print(trie_t * trie) {
    __trie_print_node(trie, trie->root, 0);
}

void trie_print_state(trie_t * trie) {
    context_t * context = &(trie->context);
    symbol_t symbol = context->entries[0].symbol;

    printf("%c: ", symbol);
    printf("[ %c | %.4f ] ", symbol, 1.0*trie->character_probability.numerator/trie->character_probability.denominator);
    for (unsigned int i = 0; i < trie->order; i++) {
        context_entry_t * entry = &(context->entries[i]);
        if (entry->probability.denominator == 0) {
            printf("[ - | - ] ");
        } else {
            printf("[ ");
            if (entry->escape) {
                printf("ESC");
            } else {
                for (unsigned int j = i; j > 0; j--)
                    printf("%c", (context->entries[j].symbol==0?' ':context->entries[j].symbol));
                printf("->%c", symbol);
            }
            printf(" | %.4f ] ", 1.0*entry->probability.numerator/entry->probability.denominator);
        }
    }
    printf("\n");
    trie_print(trie);
}

int trie_dump(trie_t * trie, char * filename) {
    return 0;
}

int trie_load(trie_t * trie, char * filename) {
    return 0;
}

void trie_clear_context(trie_t * trie) {
    context_t * context = &trie->context;
    for (unsigned int i = 0; i < trie->order; i++) {
        context->entries[i].probability.numerator = 0;
        context->entries[i].probability.denominator = 0;
        context->entries[i].escape = 0;
    }
}

void trie_clear_escapes(trie_t * trie) {
    memset(trie->context.escape_counts, 0, sizeof(unsigned long) * trie->order);
    memset(&(trie->context.avg_escape_count), 0, sizeof(fraction_t));
}

fraction_t trie_get_probability_encoding(trie_t * trie) {
    fraction_t probability;
    context_entry_t * entry;
    int i;

    probability.numerator = 1;
    probability.denominator = 1;

    for (i = (trie->order - 1); i >= 0; i--) {
        entry = &(trie->context.entries[i]);
        if (entry->probability.denominator == 0) {
            continue;
        }
        probability.numerator *= entry->probability.numerator;
        probability.denominator *= entry->probability.denominator;
        if (entry->escape == 0) {
            break;
        }
    }

    if (i == -1 && (entry->probability.denominator == 0 || entry->escape == 1)) {
        probability.numerator *= trie->character_probability.numerator;
        probability.denominator *= trie->character_probability.denominator;
    }

    return probability;
}

double trie_get_bit_encoding(trie_t * trie) {
    fraction_t probability = trie_get_probability_encoding(trie);
    return -log(1.0*probability.numerator/probability.denominator)/log(2);
}

// Updater function
void trie_add(trie_t * trie, symbol_t symbol) {
    node_i x, y, z, s, a, b;
    node_t * root;

    // initializer to make the PTR macro work
    root = trie->list->data;

    // housekeeping
    trie_clear_context(trie);
    __trie_update_context(trie, symbol);
    trie->context.avg_escape_count.denominator++;
    
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

    __trie_update_probabilities(trie, y, symbol);
    // add s as a new child node of y OR increment the count by 1
    s = __trie_getadd_symbol_child(trie, y, symbol);
    root = trie->list->data;         // in case root got stale after malloc
    trie->base_ptr = s;              // set the base to point to s
    a = s;

    // step 2
    while (PTR(y)->vine != 0) {          // repeat while y has a vine
        z = PTR(y)->vine;                // follow the vine pointer from y to z
        __trie_update_probabilities(trie, z, symbol);
        // add s as a new child node of z OR increment the count by 1
        s = __trie_getadd_symbol_child(trie, z, symbol);
        root = trie->list->data;         // in case root got stale after malloc
        b = s;
        PTR(a)->vine = b;                // add a vine to the previous node
        a = b;                           // set a to b (shift context)
        y = z;                           // set y to z (shift context)
    }

    if (PTR(a)->vine == 0) {
        PTR(a)->vine = trie->root;       // add a vine back to root
    }

}

// prints the trie in a pretty fashion
void __trie_print_node(trie_t * trie, node_i node, unsigned int level) {
    node_t * root;

    // initializer to make the PTR macro work
    root = trie->list->data;

    if (node == 0) {
        printf("\n");
        return;
    }
    // black magic here
    printf("%s-- (%3c,%03lu) ", (level==0?" ":"|"), (node==1?' ':PTR(node)->symbol), PTR(node)->count);
    printf("|-- (ESC,%03lu)\n", PTR(node)->child_escape_count);
    for (unsigned int i = 0; i < level+1; i++) {
        printf("%14s", " ");
    }
    // print all the children nodes on one line
    __trie_print_node(trie, PTR(node)->down, level+1);
    // more black magic here
    for (unsigned int i = 0; i < level; i++) {
        printf("%14s", " ");
    }
    // print all sibling nodes on new lines
    __trie_print_node(trie, PTR(node)->right, level);
}

// updates the trie context
void __trie_update_context(trie_t * trie, symbol_t symbol) {
    context_t * context = &trie->context;
    for (int i = (trie->order - 2); i >= 0; i--) {
        context->entries[i+1] = context->entries[i];
    }
    context->entries[0].symbol = symbol;
    context->entries[0].probability.numerator = 0;
    context->entries[0].probability.denominator = 0;
    context->entries[0].escape = 0;
}

void __trie_update_probabilities(trie_t * trie, node_i parent, symbol_t symbol) {
    unsigned int order;
    fraction_t symbol_probability, escape_probability;
    node_t * root;

    root = trie->list->data;
    
    order = __trie_get_node_level(trie, parent);
    symbol_probability = __node_get_symbol_probability(root, parent, symbol);
    escape_probability = __node_get_escape_probability(root, parent);
    if (symbol_probability.denominator == 0) {
        if (escape_probability.denominator == 0) {
            escape_probability.denominator = 1;
            escape_probability.numerator = 1;
        }
        trie->context.entries[order].escape = 1;
        trie->context.entries[order].probability = escape_probability;
        trie->context.escape_counts[order]++;
        trie->context.avg_escape_count.numerator++;
        PTR(parent)->child_escape_count++;
    } else {
        trie->context.entries[order].escape = 0;
        trie->context.entries[order].probability = symbol_probability;
    }
}

// finds or creates the relevant symbol in an order n context in the trie
node_i __trie_getadd_symbol_child(trie_t * trie, node_i node, symbol_t symbol) {
    node_i child, ni, result;
    node_t * root;

    // initializer to make the PTR macro work
    root = trie->list->data;

    child = PTR(node)->down;
    if (child == 0) {
        // if there are no children nodes, then create one
        child = __node_malloc(trie->list);
        root = trie->list->data; // in case root is stale after malloc
        PTR(node)->down = child;
        PTR(child)->symbol = symbol;
        PTR(child)->count = 1;
        return child;
    }

    // initialize the result node id to 0
    result = 0;
    // search among children nodes to find the symbol in this context
    for (ni = child; ni != 0; ni = PTR(ni)->right) {
        
        // if the symbol was found in this context...
        if (PTR(ni)->symbol == symbol) {
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
    // create the node and set the fields
    result = __node_malloc(trie->list);
    root = trie->list->data; // in case root is stale after malloc
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
    node_t * root;

    // initializer to make the PTR macro work
    root = trie->list->data;

    // if the vine is not set, technically is undefined
    if (PTR(node)->vine == 0) {
        return 0;
    }

    // traverse the trie and increment the count
    for (; trie->root != node; node = PTR(node)->vine, count++);

    // return the count
    return count;
}


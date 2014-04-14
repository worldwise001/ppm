#include "node.h"

unsigned long __node_get_single_child_count(node_t * root, node_i node, symbol_t symbol) {
    for (node_i i = PTR(node)->down; i != 0; i = PTR(i)->right) {
        if (PTR(i)->symbol == symbol) {
            return PTR(i)->count;
        }
    }
    return 0;
}

unsigned long __node_get_total_child_count(node_t * root, node_i node) {
    unsigned long count = PTR(node)->child_escape_count;
    for (node_i i = PTR(node)->down; i != 0; i = PTR(i)->right) {
        count += PTR(i)->count;
    }
    return count;
}

fraction_t __node_get_symbol_probability(node_t * root, node_i node, symbol_t symbol) {
    fraction_t probability;

    probability.numerator = __node_get_single_child_count(root, node, symbol);
    if (probability.numerator == 0) {
        probability.denominator = 0;
        return probability;
    }
    probability.denominator = __node_get_total_child_count(root, node);
    return probability;
}

fraction_t __node_get_escape_probability(node_t * root, node_i node) {
    fraction_t probability;
    probability.numerator = PTR(node)->child_escape_count;
    probability.denominator = __node_get_total_child_count(root, node);
    return probability;
}


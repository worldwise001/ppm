#include "trie.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    char * ppm_test_str = "assanissimassa";
    trie_t * trie = trie_create(3);
    for (unsigned int i = 0; i < strlen(ppm_test_str); i++) {
        trie_add(trie, ppm_test_str[i]);
        fraction_t pr = trie_get_probability_encoding(trie);
        double bits = trie_get_bit_encoding(trie);
        printf("%c encoded with probability %lu/%lu or %5.2f bits\n", ppm_test_str[i], pr.numerator, pr.denominator, bits);
    }
    trie_print_state(trie);
    trie_t * dup = trie_duplicate(trie);
    trie_destroy(trie);
    trie_print_state(dup);
    trie_destroy(dup);
    return 0;
}

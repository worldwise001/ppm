#include "trie.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    char * ppm_test_str = "assanissimassa";
    trie_t * trie = trie_create(3);
    fraction_t fraction;
    for (unsigned int i = 0; i < strlen(ppm_test_str); i++) {
        fraction = trie_add(trie, ppm_test_str[i]);
        printf("%c has probability %d/%d\n", ppm_test_str[i], fraction.numerator, fraction.denominator);
    }
    trie_print(trie);
    trie_destroy(trie);
    return 0;
}

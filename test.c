#include "trie.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    char * ppm_test_str = "assanissimassa";
    trie_t * trie = trie_create(3);
    for (unsigned int i = 0; i < strlen(ppm_test_str); i++) {
        trie_add(trie, ppm_test_str[i]);
        trie_print_state(trie);
    }
    //trie_print(trie);
    trie_destroy(trie);
    return 0;
}

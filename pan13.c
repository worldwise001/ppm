#include "trie.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }

    DIR * dir = opendir(argv[1]);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open directory: %s\n", strerror(errno));
        return 1;
    }

    char * path = NULL;
    char * known = NULL, * tmp = NULL, * unknown = NULL, ** datptr = NULL, * total = NULL;
    size_t known_size = 0, unknown_size = 0, * size = NULL;
    struct dirent * dir_entry;
    struct stat stat_entry;
    FILE * file;
    while ((dir_entry = readdir(dir)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
            continue;

        if (strncmp(dir_entry->d_name, "known", 5) == 0) {
            datptr = &known;
            size = &known_size;
        } else { 
            datptr = &unknown;
            size = &unknown_size;
        }

        path = malloc(strlen(argv[1]) + 1 + strlen(dir_entry->d_name) + 1);
        sprintf(path, "%s/%s", argv[1], dir_entry->d_name);
        stat(path, &stat_entry);

        tmp = realloc(*datptr, (*size) + stat_entry.st_size + 1);

        if (tmp == NULL) {
            fprintf(stderr, "Unable to realloc memory: %s\n", strerror(errno));
            return 1;
        }

        *datptr = tmp;
        tmp = (*datptr) + (*size);
        *size += stat_entry.st_size;

        file = fopen(path, "r");
        fread(tmp, 1, stat_entry.st_size, file);
        fclose(file);

        tmp[stat_entry.st_size] = '\0';

        free(path);
        path = NULL;
    }

    total = malloc(known_size+unknown_size+1);
    sprintf(total, "%s%s", known, unknown);

    trie_t * trie = trie_create(5);

    double train_bits = 0, test_bits = 0, vanilla_bits = 0;
    double train_ent = 0, test_ent = 0, vanilla_ent = 0;
    double t = 0;
    fraction_t p;
    for (unsigned int i = 0; i < strlen(known); i++) {
        trie_add(trie, known[i]);
        train_bits += trie_get_bit_encoding(trie);
        p = trie_get_probability_encoding(trie);
        t = (1.0*p.numerator/p.denominator)*log(1.0*p.numerator/p.denominator);
        train_ent -= t;
    }
    for (unsigned int i = 0; i < strlen(unknown); i++) {
        trie_add(trie, unknown[i]);
        test_bits += trie_get_bit_encoding(trie);
        p = trie_get_probability_encoding(trie);
        t = (1.0*p.numerator/p.denominator)*log(1.0*p.numerator/p.denominator);
        test_ent -= t;
    }
    trie_destroy(trie);
    trie = trie_create(5);
    for (unsigned int i = 0; i < strlen(unknown); i++) {
        trie_add(trie, unknown[i]);
        vanilla_bits += trie_get_bit_encoding(trie);
        p = trie_get_probability_encoding(trie);
        t = (1.0*p.numerator/p.denominator)*log(1.0*p.numerator/p.denominator);
        vanilla_ent -= t;
    }
    printf("Training text:       %.4f bits, ratio = %.4f, entropy = %.4f\n", train_bits, train_bits/(known_size*8), train_ent);
    printf("Test (model) text:   %.4f bits, ratio = %.4f, entropy = %.4f\n", test_bits, test_bits/(unknown_size*8), test_ent);
    printf("Test (vanilla) text: %.4f bits, ratio = %.4f, entropy = %.4f\n", vanilla_bits, vanilla_bits/(unknown_size*8), vanilla_ent);
    printf("Ratio difference:    %.4f\n", (vanilla_bits-test_bits)/(unknown_size*8));
    printf("\n");
    trie_destroy(trie);

    free(known);
    free(unknown);
    return 0;
}

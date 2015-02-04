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

#define PPM_ORDER 5

void print_usage(char * argv0) {
    fprintf(stderr, "Usage: %s <training directory> <test directory> <output directory>", argv0);
}

trie_t * train(char * train_directory, size_t * corpus_size, double * train_bits, double * train_ent) {
    DIR * dir = opendir(train_directory);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open training directory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    trie_t * trie = trie_create(PPM_ORDER);

    struct dirent * dir_entry;
    struct stat stat_entry;
    FILE * file;
    char * corpus = NULL, * tmp = NULL, * path = NULL;

    *corpus_size = 0;

    while ((dir_entry = readdir(dir)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
            continue;

        path = malloc(strlen(train_directory) + 1 + strlen(dir_entry->d_name) + 1);
        sprintf(path, "%s/%s", train_directory, dir_entry->d_name);
        stat(path, &stat_entry);

        tmp = realloc(corpus, *corpus_size + stat_entry.st_size + 1);

        if (tmp == NULL) {
            fprintf(stderr, "Unable to realloc memory: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        corpus = tmp;
        tmp = corpus + *corpus_size;
        *corpus_size += stat_entry.st_size;

        file = fopen(path, "r");
        fread(tmp, 1, stat_entry.st_size, file);
        fclose(file);

        tmp[stat_entry.st_size] = '\0';

        free(path);
        path = NULL;
    }
    closedir(dir);

    *train_bits = 0;
    *train_ent = 0;
    double t = 0;
    fraction_t p;
    for (unsigned int i = 0; i < strlen(corpus); i++) {
        trie_add(trie, corpus[i]);
        *train_bits += trie_get_bit_encoding(trie);
        p = trie_get_probability_encoding(trie);
        t = (1.0*p.numerator/p.denominator)*log(1.0*p.numerator/p.denominator);
        *train_ent -= t;
    }
    return trie;
}

void test(char * test_directory, char * output_path, trie_t * trie) {
    DIR * dir = opendir(test_directory);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open training directory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct dirent * dir_entry;
    struct stat stat_entry;
    FILE * file, * output;
    char * corpus = NULL, * path = NULL;
    size_t corpus_size = 0;

    output = fopen(output_path, "w");

    while ((dir_entry = readdir(dir)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
            continue;

        path = malloc(strlen(test_directory) + 1 + strlen(dir_entry->d_name) + 1);
        sprintf(path, "%s/%s", test_directory, dir_entry->d_name);
        stat(path, &stat_entry);

        corpus = malloc(stat_entry.st_size + 1);

        if (corpus == NULL) {
            fprintf(stderr, "Unable to realloc memory: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        corpus_size = stat_entry.st_size;
        file = fopen(path, "r");
        fread(corpus, 1, stat_entry.st_size, file);
        fclose(file);

        corpus[stat_entry.st_size] = '\0';

        free(path);
        path = NULL;

        double test_bits = 0, vanilla_bits = 0;
        double test_ent = 0, vanilla_ent = 0;
        double test_ratio = 0, vanilla_ratio = 0;
        double t = 0;
        fraction_t p;
        trie_t * dup = trie_duplicate(trie);
        for (unsigned int i = 0; i < strlen(corpus); i++) {
            trie_add(dup, corpus[i]);
            test_bits += trie_get_bit_encoding(dup);
            p = trie_get_probability_encoding(dup);
            t = (1.0*p.numerator/p.denominator)*log(1.0*p.numerator/p.denominator);
            test_ent -= t;
        }
        test_ratio = test_bits/(corpus_size*8);
        trie_destroy(dup); 
        dup = trie_create(PPM_ORDER);
        for (unsigned int i = 0; i < strlen(corpus); i++) {
            trie_add(dup, corpus[i]);
            vanilla_bits += trie_get_bit_encoding(dup);
            p = trie_get_probability_encoding(dup);
            t = (1.0*p.numerator/p.denominator)*log(1.0*p.numerator/p.denominator);
            vanilla_ent -= t;
        }
        trie_destroy(dup);
        vanilla_ratio = vanilla_bits/(corpus_size*8);
        fprintf(output, "%s,%f,%f,%f,%f,%f,%f,%f\n", dir_entry->d_name, test_bits, vanilla_bits, test_ent, vanilla_ent, test_ratio, vanilla_ratio, (vanilla_ratio-test_ratio));
    }
    closedir(dir);
    fclose(output);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    double train_bits = 0, train_ent = 0;
    size_t corpus_size = 0;
    trie_t * trie = NULL;

    trie = train(argv[1], &corpus_size, &train_bits, &train_ent);
    test(argv[2], argv[3], trie);

    trie_destroy(trie);

    return 0;
}

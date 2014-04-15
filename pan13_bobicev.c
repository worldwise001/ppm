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

#define WORD_SPLIT 350

char ** split_text(char* text) {
    int whitespace_count = 0;
    for (int i = 0; i < strlen(text); i++) {
        if (text[i] == '\n' || text[i] == ' ' || text[i] == '\t' || text[i] == '\r')
            whitespace_count++;
    }

    int split_count = whitespace_count/WORD_SPLIT + 2;
    char ** splitted = malloc(sizeof(char*)*split_count);
    whitespace_count = 0;
    int splix = 0;
    char * ptr1 = text, * ptr2 = text;
    while (*ptr2 != '\0') {
        if (*ptr2 == '\n' || *ptr2 == ' ' || *ptr2 == '\t' || *ptr2 == '\r')
            whitespace_count++;
        ptr2++;
        if ((whitespace_count >= WORD_SPLIT) ||  (*ptr2 == '\0')) {
            splitted[splix] = malloc(ptr2-ptr1 + 1);
            memcpy(splitted[splix], ptr1, ptr2-ptr1);
            splitted[splix][ptr2-ptr1] = '\0';
            ptr1 = ptr2;
            splix++;
            whitespace_count = 0;
        }
    }
    splitted[splix] = NULL;
    return splitted;
}

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

        tmp = realloc(*datptr, (*size) + stat_entry.st_size + 2);

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

        tmp[stat_entry.st_size] = '\n';
        tmp[stat_entry.st_size+1] = '\0';

        free(path);
        path = NULL;
    }
    closedir(dir);

    known_size++;
    unknown_size++;

    total = malloc(known_size+unknown_size+2);
    sprintf(total, "%s\n%s", known, unknown);

    fraction_t char_pr = calculate_char_probability(total);
    free(total);

    char ** t1, ** t2, ** ptr;
    unsigned int order = 5;

    t1 = split_text(known);
    t2 = split_text(unknown);
    
/*
    for (ptr = t1; *ptr != NULL; ptr++) {
        printf("known text is %lu bytes\n", strlen(*ptr));
    }

    for (ptr = t2; *ptr != NULL; ptr++) {
        printf("unknown text is %lu bytes\n", strlen(*ptr));
    }
*/
    printf("Type, Same, Cross\n");

    for (ptr = t1; *ptr != NULL; ptr++) {
        trie_t * trie;
        double entropy = 0;
        fraction_t pr;

        int train_size = 0;
        for (char ** ptr2 = t1; *ptr2 != NULL; ptr2++) {
            if (ptr2 != ptr)
                train_size += strlen(*ptr2);
        }
        char * train_chunk = malloc(train_size+1), * chunk_ptr = train_chunk;
        for (char ** ptr2 = t1; *ptr2 != NULL; ptr2++) {
            if (ptr2 != ptr) {
                sprintf(chunk_ptr, "%s", *ptr2);
                chunk_ptr += strlen(*ptr2);
            }
        }
        trie = trie_create(order);
        trie->character_probability = char_pr;
        entropy = 0;
        for (unsigned i = 0; i < strlen(train_chunk); i++) {
            trie_add(trie, train_chunk[i]);
        }
        for (unsigned i = 0; i < strlen(*ptr); i++) {
            trie_add(trie, (*ptr)[i]);
            pr = trie_get_probability_encoding(trie);
            entropy -= (1.0*pr.numerator/pr.denominator)*log(1.0*pr.numerator/pr.denominator);
        }
        printf("K, %.4f,", entropy);
        trie_destroy(trie);
        free(train_chunk);

        entropy = 0;
        trie = trie_create(order);
        trie->character_probability = char_pr;
        for (unsigned i = 0; i < strlen(unknown); i++) {
             trie_add(trie, unknown[i]);
        }
        for (unsigned i = 0; i < strlen(*ptr); i++) {
            trie_add(trie, (*ptr)[i]);
            pr = trie_get_probability_encoding(trie);
            entropy -= (1.0*pr.numerator/pr.denominator)*log(1.0*pr.numerator/pr.denominator);
        }
        printf(" %.4f\n", entropy);
        trie_destroy(trie);
    }

    for (ptr = t2; *ptr != NULL; ptr++) {
        trie_t * trie;
        double entropy = 0;
        fraction_t pr;

        int train_size = 0;
        for (char ** ptr2 = t2; *ptr2 != NULL; ptr2++) {
            if (ptr2 != ptr)
                train_size += strlen(*ptr2);
        }
        char * train_chunk = malloc(train_size+1), * chunk_ptr = train_chunk;
        for (char ** ptr2 = t2; *ptr2 != NULL; ptr2++) {
            if (ptr2 != ptr) {
                sprintf(chunk_ptr, "%s", *ptr2);
                chunk_ptr += strlen(*ptr2);
            }
        }
        trie = trie_create(order);
        trie->character_probability = char_pr;
        entropy = 0;
        for (unsigned i = 0; i < strlen(train_chunk); i++) {
            trie_add(trie, train_chunk[i]);
        }
        for (unsigned i = 0; i < strlen(*ptr); i++) {
            trie_add(trie, (*ptr)[i]);
            pr = trie_get_probability_encoding(trie);
            entropy -= (1.0*pr.numerator/pr.denominator)*log(1.0*pr.numerator/pr.denominator);
        }
        printf("U, %.4f,", entropy);
        trie_destroy(trie);
        free(train_chunk);

        entropy = 0;
        trie = trie_create(order);
        trie->character_probability = char_pr;
        for (unsigned i = 0; i < strlen(known); i++) {
             trie_add(trie, known[i]);
        }
        for (unsigned i = 0; i < strlen(*ptr); i++) {
            trie_add(trie, (*ptr)[i]);
            pr = trie_get_probability_encoding(trie);
            entropy -= (1.0*pr.numerator/pr.denominator)*log(1.0*pr.numerator/pr.denominator);
        }
        printf(" %.4f\n", entropy);
        trie_destroy(trie);
    }

    for (ptr = t1; *ptr != NULL; ptr++) {
        free(*ptr);
    }

    for (ptr = t2; *ptr != NULL; ptr++) {
        free(*ptr);
    }

    free(t1);
    free(t2);

    free(known);
    free(unknown);
    return 0;
}

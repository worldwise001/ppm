#include "data.h"

#include <string.h>
#include <stdio.h>

fraction_t calculate_char_probability(char * text) {
    unsigned char test[256];
    fraction_t probability;
    int i;

    memset(test, 0, sizeof(char)*256);
    memset(&probability, 0, sizeof(fraction_t));

    while ((*text) != '\0') {
        i = 0x00FF && (*text);
        test[i] = 1;
        text++;
    }

    for (i = 0; i < 256; i++) {
        probability.denominator += test[i];
    }

    probability.numerator = 1;
    return probability;
}

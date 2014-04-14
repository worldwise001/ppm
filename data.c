#include "data.h"

#include <string.h>

fraction_t calculate_char_probability(char * text) {
    unsigned char test[256];
    fraction_t probability;
    int i;

    memset(test, 0, sizeof(char)*256);
    memset(&probability, 0, sizeof(fraction_t));

    while (*text != '\0') {
        test[(int)(*text)] = 1;
        text++;
    }

    for (i = 0; i < 256; i++) {
        probability.denominator += test[i];
    }

    probability.numerator = 1;
    return probability;
}

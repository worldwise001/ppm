#ifndef PPM_DATA_H_
#define PPM_DATA_H_

#define PPM_ORDER 5

typedef unsigned char symbol_t;
typedef unsigned int code_t;

struct __fraction_struct__;
typedef struct __fraction_struct__ fraction_t;

struct __fraction_struct__ {
    unsigned long numerator;
    unsigned long denominator;
};

// Calculates the character probability for a given text
fraction_t calculate_char_probability(char * text);

#endif // PPM_DATA_H_

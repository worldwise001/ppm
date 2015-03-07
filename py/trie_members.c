#include "pytrie.h"

PyObject *
Trie_get_probability_encoding(Trie *self, void *closure)
{
    fraction_t pr = trie_get_probability_encoding(self->trie);
    return PyFloat_FromDouble((1.0)*pr.numerator/pr.denominator);
}

PyObject *
Trie_get_bit_encoding(Trie *self, void *closure)
{
    return PyFloat_FromDouble(trie_get_bit_encoding(self->trie));
}

PyObject *
Trie_get_order(Trie *self, void *closure)
{
    return PyInt_FromLong(self->trie->order);
}


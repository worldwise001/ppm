#ifndef TRIE_PY_H_
#define TRIE_PY_H_

#include <Python.h>
#include <structmember.h>
#include <trie.h>

typedef struct {
    PyObject_HEAD
    trie_t * trie;
} Trie;

// Python object scaffolding
PyObject * Trie_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int Trie_init(Trie *self, PyObject *args, PyObject *kwds);
void Trie_dealloc(Trie* self);

// Python object methods
PyObject * Trie_duplicate(Trie *self);
PyObject * Trie_clear_context(Trie *self);
PyObject * Trie_clear_escapes(Trie *self);
PyObject * Trie_add(Trie *self, PyObject *args, PyObject *keywds);

// Python object members
PyObject * Trie_get_probability_encoding(Trie *self, void *closure);
PyObject * Trie_get_bit_encoding(Trie *self, void *closure);
PyObject * Trie_get_order(Trie *self, void *closure);

#endif // TRIE_PY_H_

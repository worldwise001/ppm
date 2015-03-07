#include "pytrie.h"
#include <stdio.h>

extern PyTypeObject TrieType;

PyObject * Trie_duplicate(Trie* self)
{
    PyObject * other;
    Trie * other_trie;
    PyObject *argList = Py_BuildValue("(I)", self->trie->order);
    other = PyObject_CallObject((PyObject *) &TrieType, argList);
    other_trie = (Trie *)other;
    Py_DECREF(argList);
    if (other_trie != NULL) {
        trie_destroy(other_trie->trie);
        other_trie->trie = NULL;
        other_trie->trie = trie_duplicate(self->trie);
    }
    return other;
}

PyObject * Trie_clear_context(Trie* self)
{
    trie_clear_context(self->trie);
    Py_RETURN_NONE;
}

PyObject * Trie_clear_escapes(Trie* self)
{
    trie_clear_escapes(self->trie);
    Py_RETURN_NONE;
}

PyObject * Trie_add(Trie *self, PyObject *args, PyObject *keywds)
{
    unsigned char symbol = 0;
    static char *kwlist[] = {"symbol", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "c", kwlist,
                                     &symbol)) {
        PyErr_Print();
        Py_RETURN_NONE;
    }
    trie_add(self->trie, symbol); 
    Py_RETURN_NONE;
}


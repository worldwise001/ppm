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

PyObject * Trie___getstate__(Trie *self)
{
    PyObject * tmp, * tmp1;
    PyObject * pickle = PyDict_New();
    
    // pack order
    tmp = PyInt_FromLong(self->trie->order);
    PyDict_SetItemString(pickle, "order", tmp);
    // pack root
    tmp = PyInt_FromLong(self->trie->root);
    PyDict_SetItemString(pickle, "root", tmp);
    // pack base_ptr
    tmp = PyInt_FromLong(self->trie->base_ptr);
    PyDict_SetItemString(pickle, "base_ptr", tmp);
    
    // pack nodelist
    tmp1 = PyDict_New();
    PyDict_SetItemString(pickle, "list", tmp1);
    // pack nodelist data
    tmp = PyByteArray_FromStringAndSize((char *)(self->trie->list->data), sizeof(node_t)*(self->trie->list->size));
    PyDict_SetItemString(tmp1, "data", tmp);
    // pack nodelist ptr
    tmp = PyInt_FromLong(self->trie->list->ptr);
    PyDict_SetItemString(tmp1, "ptr", tmp);
    // pack nodelist size
    tmp = PyInt_FromLong(self->trie->list->size);
    PyDict_SetItemString(tmp1, "size", tmp);
    
    // pack context
    tmp1 = PyDict_New();
    PyDict_SetItemString(pickle, "context", tmp1);
    // pack context entries
    tmp = PyByteArray_FromStringAndSize((char *)(self->trie->context.entries), sizeof(context_entry_t)*(self->trie->order));
    PyDict_SetItemString(tmp1, "entries", tmp);
    // pack context escape counts
    tmp = PyByteArray_FromStringAndSize((char *)(self->trie->context.escape_counts), sizeof(unsigned long)*(self->trie->order));
    PyDict_SetItemString(tmp1, "escape_counts", tmp);
    // pack context avg escape count numerator
    tmp = PyInt_FromLong(self->trie->context.avg_escape_count.numerator);
    PyDict_SetItemString(tmp1, "avg_escape_count_numerator", tmp);
    tmp = PyInt_FromLong(self->trie->context.avg_escape_count.denominator);
    PyDict_SetItemString(tmp1, "avg_escape_count_denominator", tmp);
    
    return pickle;
}

PyObject * Trie___setstate__(Trie *self, PyObject *args, PyObject *keywds)
{
    PyObject * pickle = NULL;
    PyObject * tmp, * tmp1;
    char * data;
    size_t size;
    static char *kwlist[] = {"state", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "O", kwlist,
                                     &pickle)) {
        PyErr_Print();
        Py_RETURN_NONE;
    }
    tmp = PyDict_GetItemString(pickle, "order");
    
    if (self->trie != NULL) {
        trie_destroy(self->trie);
        self->trie = NULL;
    }
    
    self->trie = trie_create((unsigned int)(PyInt_AsLong(tmp)));
    
    tmp = PyDict_GetItemString(pickle, "root");
    self->trie->root = (unsigned int)(PyInt_AsLong(tmp));
    tmp = PyDict_GetItemString(pickle, "base_ptr");
    self->trie->base_ptr = (unsigned int)(PyInt_AsLong(tmp));
    
    tmp1 = PyDict_GetItemString(pickle, "list");
    tmp = PyDict_GetItemString(tmp1, "ptr");
    self->trie->list->ptr = PyInt_AsLong(tmp);
    
    free(self->trie->list->data);
    tmp = PyDict_GetItemString(tmp1, "size");
    size = PyInt_AsLong(tmp);
    self->trie->list->data = malloc(size*sizeof(node_t));
    tmp = PyDict_GetItemString(tmp1, "data");
    data = PyByteArray_AsString(tmp);
    memcpy(self->trie->list->data, data, size*sizeof(node_t));
    
    tmp1 = PyDict_GetItemString(pickle, "context");
    tmp = PyDict_GetItemString(tmp1, "entries");
    data = PyByteArray_AsString(tmp);
    memcpy(self->trie->context.entries, data, (self->trie->order)*(sizeof(context_entry_t)));
    tmp = PyDict_GetItemString(tmp1, "escape_counts");
    data = PyByteArray_AsString(tmp);
    memcpy(self->trie->context.escape_counts, data, (self->trie->order)*(sizeof(unsigned long)));
    tmp = PyDict_GetItemString(tmp1, "avg_escape_count_numerator");
    self->trie->context.avg_escape_count.numerator = PyInt_AsLong(tmp);
    tmp = PyDict_GetItemString(tmp1, "avg_escape_count_denominator");
    self->trie->context.avg_escape_count.denominator = PyInt_AsLong(tmp);
    
    Py_RETURN_NONE;
}

PyObject * Trie___reduce__(Trie *self) {
    PyObject * tuple;
    PyObject * argList;
    PyObject * state;
    
    tuple = PyTuple_New(3);
    
    PyTuple_SetItem(tuple, 0, (PyObject *) &TrieType);
    
    argList = Py_BuildValue("(I)", self->trie->order);
    PyTuple_SetItem(tuple, 1, argList);
    
    state = Trie___getstate__(self);
    PyTuple_SetItem(tuple, 2, state);
    
    return tuple;
}


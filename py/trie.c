#include "pytrie.h"

PyObject * Trie_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Trie *self;
    self = (Trie *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->trie = NULL;
    }
    return (PyObject *)self;
}

int Trie_init(Trie *self, PyObject *args, PyObject *kwds)
{
    unsigned int order = 5;
    static char *kwlist[] = {"order", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|I", kwlist, &order)) {
        PyErr_Print();
        return -1;
    }
    if (self == NULL) {
        return -1;
    } else {
        self->trie = trie_create(order);
        return 0;
    }
}

void Trie_dealloc(Trie* self)
{
    trie_destroy(self->trie);
    self->ob_type->tp_free((PyObject*)self);
}

PyMemberDef Trie_members[] = {
    {NULL}  /* Sentinel */
};

PyGetSetDef Trie_getseters[] = {
    {"probability_encoding", 
     (getter)Trie_get_probability_encoding, NULL,
     "probability encoding of the trie",
     NULL},
    {"bit_encoding", 
     (getter)Trie_get_bit_encoding, NULL,
     "bit encoding of the trie",
     NULL},
    {"order", 
     (getter)Trie_get_order, NULL,
     "order of the trie",
     NULL},
    {NULL}  /* Sentinel */
};

PyMethodDef Trie_methods[] = {
    {"duplicate", (PyCFunction)Trie_duplicate, METH_NOARGS, "Duplicate the internal state of the trie"},
//    {"print", (PyCFunction)Trie_print, METH_NOARGS, "Print the trie"},
//    {"print_state", (PyCFunction)Trie_print_state, METH_NOARGS, "Print the trie state"},
    {"clear_context", (PyCFunction)Trie_clear_context, METH_NOARGS, "Clear the context of the trie"},
    {"clear_escapes", (PyCFunction)Trie_clear_escapes, METH_NOARGS, "Clear trie escapes"},
    {"add", (PyCFunction)Trie_add, METH_VARARGS, "Add a character to the trie"},
    {"__getstate__", (PyCFunction)Trie___getstate__, METH_NOARGS, "__getstate__"},
    {"__setstate__", (PyCFunction)Trie___setstate__, METH_VARARGS, "__setstate__"},
    {NULL}  /* Sentinel */
};

PyTypeObject TrieType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "ppm.Trie",                /*tp_name*/
    sizeof(Trie),              /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Trie_dealloc,  /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Trie objects",            /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Trie_methods,              /* tp_methods */
    Trie_members,              /* tp_members */
    Trie_getseters,            /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Trie_init,       /* tp_init */
    0,                         /* tp_alloc */
    Trie_new,                  /* tp_new */
};

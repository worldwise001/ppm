#include "pytrie.h"

extern PyTypeObject TrieType;

PyMethodDef ppm_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initppm(void) 
{
    PyObject* m;

    TrieType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&TrieType) < 0)
        return;

    m = Py_InitModule3("ppm", ppm_methods, "methods for the PPM classifier");

    Py_INCREF(&TrieType);
    PyModule_AddObject(m, "Trie", (PyObject *)&TrieType);
}


#include <Python.h>
#include <stdlib.h>

static PyObject *SpamError;

static PyObject * spam_system(PyObject *self, PyObject *args) {
    const char *command;
    int sts;
    if (!PyArg_ParseTuple(args, "s", &command)) {
        return NULL;
    }
    sts = system(command);
    if (sts < 0) {
        PyErr_SetString(SpamError, "System command failed");
        return NULL;
    }
    return Py_BuildValue("i", sts);
}

static PyObject * spam_testdata(PyObject *self, PyObject *args) {
    PyObject *data;
    if (!PyArg_ParseTuple(args, "O", &data)) {
        return NULL;
    }

    int size = PyList_Size(data);
    printf("size: %d\n", size);

    int * p_data = (int *)malloc(sizeof(int) * size);

    for (int i = 0; i < size; ++i) {
        *(p_data + i) = (int)PyInt_AsLong( PyList_GetItem(data, i) );
        printf("%d\n", *(p_data + i));
    }


    // return Py_BuildValue("i", 1);
    Py_RETURN_NONE;
}

static PyMethodDef SpamMethods[] = {
    {"system", spam_system,   METH_VARARGS, "Execute a shell command."},
    {"testdata", spam_testdata, METH_VARARGS, "test pass array"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initspam(void) {
    PyObject *m;

    m = Py_InitModule("spam", SpamMethods);

    if (m == NULL) {
        return;
    }

    SpamError = PyErr_NewException("spam.error", NULL, NULL);

    Py_INCREF(SpamError);

    PyModule_AddObject(m, "error", SpamError);

}



int main(int argc, char *argv[]) {
    // Pass argv[0] to the Python interpreter
    Py_SetProgramName(argv[0]);

    // Initialize the Python interpreter
    Py_Initialize();

    // Add a static module
    initspam();
}

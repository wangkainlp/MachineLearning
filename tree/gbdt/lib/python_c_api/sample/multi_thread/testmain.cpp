#include <Python.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

/*
void constructTree(int depth) {
    TreeNode* root = new TreeNode();
    
}
*/

void* thread_func(void* ptr) {
    int id = *((int*)ptr);
    for (int i = 0; i < 10; ++i) {
        printf("%d is run.\n", id);
        sleep(1);
    }
    return (void*)0;
}

static PyObject *TestmainError;

static PyObject* testmain_test(PyObject* self, PyObject* args) {
    int num = 0;
    if (!PyArg_ParseTuple(args, "i", &num)) {
        printf("error");
        return NULL;
    }

    pthread_t threads[3];
    int ids[3];

    for (int i = 0; i < 3; ++i) {
        ids[i] = i;
        int ret = pthread_create(&threads[i], NULL, thread_func, (void*)(ids + i));
        if (ret) {
            printf("creat thread error!\n");
            return Py_BuildValue("i", 1);
        }
    }

    for (int i = 0; i < 3; ++i) {
        pthread_join(threads[i], NULL);
    }

    return Py_BuildValue("i", 0);
}

static PyMethodDef TestmainMethods[] = {
    {"testfunc", testmain_test, METH_VARARGS, "test fuc"},
    {NULL,       NULL,      0,            NULL}
};

PyMODINIT_FUNC inittestmain(void) {
    printf("lib init begin\n");

    PyObject *m;
    m = Py_InitModule("testmain", TestmainMethods);

    if (m == NULL) {
        return;
    }

    // error
    TestmainError = PyErr_NewException("tree.error", NULL, NULL);

    Py_INCREF(TestmainError);

    PyModule_AddObject(m, "error", TestmainError);

    printf("lib init end\n");
}

int main(int argc, char* argv[]) {
    // Pass argv[0] to the Python interpreter
    Py_SetProgramName(argv[0]);

    // Initialize the Python interpreter
    Py_Initialize();

    // Add a static module
    inittestmain();

    return 0;
}

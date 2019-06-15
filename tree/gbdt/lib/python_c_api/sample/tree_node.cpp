#include <Python.h>
#include <math.h>
#include <vector>
#include "myutil.h"
#include "structmember.h"

typedef struct {
    PyObject_HEAD
    PyObject *first; /* first name */
    PyObject *last;  /* last name */
    int number;
} Noddy;

static void Noddy_dealloc(Noddy* self)
{
    Py_XDECREF(self->first);
    Py_XDECREF(self->last);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * Noddy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Noddy *self;

    self = (Noddy *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->first = PyString_FromString("");
        if (self->first == NULL) {
            Py_DECREF(self);
            return NULL;
        }

        self->last = PyString_FromString("");
        if (self->last == NULL) {
            Py_DECREF(self);
            return NULL;
        }

        self->number = 0;
    }

    return (PyObject *)self;
}

static int Noddy_init(Noddy *self, PyObject *args, PyObject *kwds)
{
    PyObject *first=NULL, *last=NULL, *tmp;

    static char *kwlist[] = {"first", "last", "number", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist,
                                      &first, &last,
                                      &self->number))
        return -1;

    if (first) {
        tmp = self->first;
        Py_INCREF(first);
        self->first = first;
        Py_XDECREF(tmp);
    }

    if (last) {
        tmp = self->last;
        Py_INCREF(last);
        self->last = last;
        Py_XDECREF(tmp);
    }

    return 0;
}


static PyMemberDef Noddy_members[] = {
    {"first",  T_OBJECT_EX, offsetof(Noddy, first),  0, "first name"},
    {"last",   T_OBJECT_EX, offsetof(Noddy, last),   0, "last name"},
    {"number", T_INT,       offsetof(Noddy, number), 0, "noddy number"},
    {NULL}  /* Sentinel */
};

static PyObject * Noddy_name(Noddy* self)
{
    static PyObject *format = NULL;
    PyObject *args, *result;

    if (format == NULL) {
        format = PyString_FromString("%s %s");
        if (format == NULL)
            return NULL;
    }

    if (self->first == NULL) {
        PyErr_SetString(PyExc_AttributeError, "first");
        return NULL;
    }

    if (self->last == NULL) {
        PyErr_SetString(PyExc_AttributeError, "last");
        return NULL;
    }

    args = Py_BuildValue("OO", self->first, self->last);
    if (args == NULL)
        return NULL;

    result = PyString_Format(format, args);
    Py_DECREF(args);

    return result;
}

class Test {
public:
    Test() {
        v = 0;
    }
    int v;
};

static PyObject * Noddy_test(Noddy* self) {
    // return Py_BuildValue("[i]", 1);
    /*
    PyObject *pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 1));//0—序号  i表示创建int型变量
    PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", 2));
    PyTuple_SetItem(pArgs, 2, Py_BuildValue("i", 3));

    return Py_BuildValue("O", pArgs);
    */

    /*
    PyObject* pRetDict = PyDict_New();
    PyDict_SetItem(pRetDict, Py_BuildValue("s", "name"), Py_BuildValue("s", "return"));
    return Py_BuildValue("O", pRetDict);
    */

    TreeNode* n1 = new TreeNode();
    n1->split = 0.0;
    n1->depth = 0;

    n1->left = new TreeNode();
    n1->left->split = 0.1;

    n1->right = new TreeNode();
    n1->right->split = 0.2;

    n1->left->left = new TreeNode();
    n1->left->left->split = 0.3;

    n1->right->right = new TreeNode();
    n1->right->right->split = 0.6;

    int tree_size = pow(2, 3) - 1;

    TreeNode* tree[tree_size];
    for (int i = 0; i < tree_size; ++i) {
        tree[i] = NULL;
    }

    tree2array(n1, tree, tree_size);
     for (int i = 0; i < tree_size; ++i) {
         if (tree[i] != NULL) {
             printf("%.2f ", tree[i]->split);
         } else {
            printf("0 ");
         }
     }
    printf("\n");

    PyObject *pArgs = PyTuple_New(tree_size);
    for (int i = 0; i < tree_size; ++i) {
        PyObject* newNode = newTreeNode(tree[i]);
        PyTuple_SetItem(pArgs, i, newNode);
        /*
        PyObject* pTmpDict = PyDict_New();
        PyDict_SetItem(pTmpDict, Py_BuildValue("s", "split"), Py_BuildValue("f", n1->split));
        PyTuple_SetItem(pArgs, i, pTmpDict);
        */
    }
    return Py_BuildValue("O", pArgs);
    // return Py_BuildValue("O", newTreeNode(n1));
}
static PyMethodDef Noddy_methods[] = {
    {"name", (PyCFunction)Noddy_name, METH_NOARGS, "Return the name, combining the first and last name" },
    {"test", (PyCFunction)Noddy_test, METH_NOARGS, "return test" },
    {NULL}  /* Sentinel */
};

static PyTypeObject NoddyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "noddy.Noddy",             /* tp_name */
    sizeof(Noddy),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Noddy_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Noddy objects",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Noddy_methods,             /* tp_methods */
    Noddy_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Noddy_init,      /* tp_init */
    0,                         /* tp_alloc */
    Noddy_new,                 /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC initnoddy(void)
{
    PyObject* m;

    if (PyType_Ready(&NoddyType) < 0)
        return;

    m = Py_InitModule3("noddy", module_methods,
                       "Example module that creates an extension type.");

    if (m == NULL)
        return;

    Py_INCREF(&NoddyType);
    PyModule_AddObject(m, "Noddy", (PyObject *)&NoddyType);
}

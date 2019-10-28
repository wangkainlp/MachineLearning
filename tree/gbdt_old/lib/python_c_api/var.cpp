#include <Python.h>
#include <vector>
using namespace std;

static double dataVarianceV1(double sp, double feaList[], double labelList[], long len, double sum) {
    long   lSize = 0;
    double lSum = 0.0;

    int i;
    for (i = 0; i < len; ++i) {
        if (feaList[i] <= sp) {
            ++lSize;
            lSum += labelList[i];
        } else {
            break;
        }
    }
    
    long   rSize = len - lSize;
    double rSum = sum - lSum;
    double var = -1.0 * ( (lSum / lSize * lSum) + (rSum / rSize * rSum) );
    return var;
}




static double dataVariance(double sp, double feaList[], double labelList[], long len) {
    long   lSize = 0;
    double lSum = 0.0;
    long   rSize = 0;
    double rSum = 0.0;

    int i;
    for (i = 0; i < len; ++i) {
        if (feaList[i] <= sp) {
            ++lSize;
            lSum += labelList[i];
        } else {
            ++rSize;
            rSum += labelList[i];
        }
    }
    double var = -1.0 * ( (lSum / lSize * lSum) + (rSum / rSize * rSum) );
    return var;
}

static PyObject* var_findLeastVar(PyObject* self, PyObject* args) {
    PyObject * spListObj;
    PyObject * feaListObj;
    PyObject * labelListObj;

    if (! PyArg_ParseTuple(args, "OOO", &spListObj, &feaListObj, &labelListObj)) {
        return NULL;
    }

    long spLen = PyList_Size(spListObj);
    long feaLen = PyList_Size(feaListObj);

    double spList[spLen];
    double feaList[feaLen];
    double labelList[feaLen];

    for (int i = 0; i < spLen; ++i) {
        spList[i] = PyFloat_AsDouble( PyList_GetItem(spListObj, i) );
    }
    for (int i = 0; i < feaLen; ++i) {
        feaList[i] = PyFloat_AsDouble( PyList_GetItem(feaListObj, i) );
        labelList[i] = PyFloat_AsDouble( PyList_GetItem(labelListObj, i) );
    }


    
    double minSp = 1000000000.0;
    double minVar = 1000000000.0; 

    double sum = 0.0;
    for (int i = 0; i < feaLen; ++i) {
        sum += labelList[i];
    }

    /*
    for (int i = 0; i < spLen; ++i) {
        // double var = dataVariance(spList[i], feaList, labelList, feaLen);
        double var = dataVarianceV1(spList[i], feaList, labelList, feaLen, sum);
        if (var < minVar) {
            minVar = var;
            minSp = spList[i];
        }
    }
    printf("run\n");
    */

    int spIdx = 0;
    double _sum = 0.0;
    for (int i = 1; i < feaLen; ++i) {
        if (spIdx > spLen) {
            break;
        }
        _sum += labelList[i - 1];
        if (feaList[i - 1] < spList[spIdx] && spList[spIdx] < feaList[i] ) {
            double rSum = sum - _sum;
            double var = -1 * ( (_sum / i * _sum) + (rSum / (feaLen - i) * rSum) );
            if (var < minVar) {
                minSp = spList[spIdx];
                minVar = var;
                ++spIdx;
            } 
        }

    }

    /*
    // vector<vector<double> > spLeft;
    vector<double> spLeft[spLen];
    for (int i = 0; i < feaLen; ++i) {
        float feaValue = feaList[i];
        for (int j = 0; j < spLen; ++j) {
            if (feaValue < spList[j]) {
                spLeft[j].push_back(feaValue);
            }
        }
        // printf("run %d\n", i);
    }

    printf("run\n");

    double sum = 0.0;
    for (int i = 0; i < feaLen; ++i) {
        sum += feaList[i];
    }

    printf("run\n");

    for (int i = 0; i < spLen; ++i) {
        int leftSize = spLeft[i].size();
        int rightSize =  feaLen - leftSize;
        double leftSum = 0.0;
        for (int j = 0; j < leftSize; ++j) { 
            leftSum += spLeft[i][j];
        }
        double rightSum = sum - leftSum;
        double curVar = -( leftSum / leftSize * leftSum + rightSum / rightSize * rightSum);
        if (curVar < minSp) {
            minVar = curVar;
            minSp = spList[i];
        }
    }
    */


    // return Py_BuildValue("d", minSp);
    return Py_BuildValue("(d, d)", minSp, minVar);
}

static char var_docs[] = "var( ) : findLeastVar\n";

static PyMethodDef var_funcs[] = {
    {"findLeastVar", (PyCFunction)var_findLeastVar, METH_VARARGS, var_docs},
    {NULL, NULL, 0, NULL}

};

PyMODINIT_FUNC initvar(void) {
    Py_InitModule3("var", var_funcs, "findLeastVar");
}

#ifndef _myutil_h_
#define _myutil_h_

#include <Python.h>
#include <cstdlib>
#include <time.h>
#include <vector> 
#include <cmath>

#include "tree_node.h"

class TrainConf {
public:
    TrainConf() {
    }

    void init(PyObject * train_conf_raw) {
        rowWidth = (int)PyInt_AsLong( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "row_width")) );
        colWidth = (int)PyInt_AsLong( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "col_width")) );
        dataSize =  (int)PyInt_AsLong( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "data_size")) );
        dimSize =  (int)PyInt_AsLong( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "dim_size")) );
        rowSample = (float)PyFloat_AsDouble( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "row_sample")) );
        colSample = (float)PyFloat_AsDouble( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "col_sample")) );
        treeSize =  (int)PyInt_AsLong( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "tree_size")) );
        shrink   = (float)PyFloat_AsDouble( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "shrink")) );
        maxDepth = (int)PyInt_AsLong( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "max_depth")) );
        minCoverNum = (int)PyInt_AsLong( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "min_cover_num")) ); 
        lambda   = (float)PyFloat_AsDouble( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "lambda")) ); 
        minGain  = (float)PyFloat_AsDouble( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "min_gain")) );
        randSeed = (int)PyInt_AsLong( PyDict_GetItem(train_conf_raw, Py_BuildValue("s", "seed")) ); 
    }

    int rowWidth;
    int colWidth; 
    int dataSize;
    int dimSize;
    float rowSample;
    float colSample;
    int treeSize;
    float shrink;
    int maxDepth;
    int minCoverNum;
    float minGain;
    float lambda;
    int randSeed;
};


struct timeval tv;

double getTime() {
    gettimeofday(&tv, NULL);
    double mm = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
    return mm;
}

// Sigmoid func
float predTransform(float val) {
    float ret = 0.0f;
    if (val >= 0.0f) {
        ret = 1.0f / (1.0f + expf(-val));
    } else {
        float e = expf(val);
        ret = e / (e + 1.0f);
    }
    // printf("val:%f ret:%f\n", val, ret);
    return ret;
    // return 1.0f / (1.0f + expf(-val));
}

#endif



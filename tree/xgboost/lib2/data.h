#include <Python.h>
#include <stdlib.h>
#include <vector>

#ifndef _data_h_
#define _data_h_

class Matrix {
public:
    Matrix(PyObject * data_raw, int _row, int _col) {
        row = _row;
        col = _col;
        span = _col;

        size = row * col;
        int size = (int)PyList_Size(data_raw);
        this->p_data = (float *)malloc(sizeof(float) * size);
        for (int i = 0; i < size; ++i) {
            this->p_data[i] = (float)PyFloat_AsDouble( PyList_GetItem(data_raw, i) );
        }

        this->p_data_row_idx = (float**)malloc(sizeof(float*) * row);

        float* ptr = this->p_data;
        for (int i = 0; i < row; ++i) {
            this->p_data_row_idx[i] = ptr;
            ptr += col;
        }
    }
    ~Matrix() {
        if (this->p_data != NULL) {
            free(this->p_data);
            this->p_data = NULL;
        }
        if (this->p_data_row_idx != NULL) {
            free(this->p_data_row_idx);
            this->p_data_row_idx = NULL;
        }
    }

    int length() {
        return this->row;
    }

    int getRange() {
        return this->span;
    }

    float* getRow(int x) {
        return *(this->p_data_row_idx + x);
    }

    float at(int x, int y) {
        return *(*(this->p_data_row_idx + x) + y);
        /*
        int idx = 0;
        idx = this->span * x + y;
        if (idx < this->size) {
            return *(p_data + idx);
        } else {
            return 0.0f;
        }
        */
    }

    float sumOfCol(int c) {
        float sum = 0.0;
        int last = c;
        for (int i = 0; i < this->col; ++i) {
            sum += *(p_data + last);
            last += this->span;
        }
        return sum;
    }

    float in(int x, int y);

private:
    int row;
    int col;
    int span;
    int size;

    float* p_data;
    float** p_data_row_idx;
};

float Matrix::in(int x, int y) {
    return this->at(x, y);
}


#endif

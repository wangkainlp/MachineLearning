#include <stdio.h>
#include <stdlib.h>

typedef struct tuple {
    float first;
    float second;
} tuple, * tuple_p;

int add_int(int, int);
float add_float(float, float);
float add_arr(float a[], int len);

int add_int(int num1, int num2){
    return num1 + num2;
}

float add_float(float num1, float num2){
    return num1 + num2;
}


float add_arr(float a[], int len) {
    float sum = 10.01;
    int i;
    int cnt = 0;
    for (i = 0; i < len; ++i) {
        sum += a[i];
        ++cnt;
    }
    return sum;
    // return len;
}

float variance(float labels[], int len) {
    if (len <= 1)
        return 0.0;
    float sum = 0.0;
    for (int i = 0; i < len; ++i) {
        sum += labels[i];
    }
    float avg = sum / len;
    float var = 0.0;
    for (int i = 0; i < len; ++i) {
        float res = labels[i] - avg;
        var += res * res;
    }
    return var;
}

float dataVariance(float labels[], float feas[], int len, float sp) {
    float left[len];
    float right[len];
    int lSize = 0;
    int rSize = 0;
    for (int i = 0; i < len; ++i) {
        if (feas[i] <= sp) {
            left[lSize] = feas[i];
            ++lSize;
        } else {
            right[rSize] = feas[i];
            ++rSize;
        }
    }
    float var = variance(left, lSize) + variance(right, rSize);
    return var;
}

tuple_p findLeastVar(float spList[], float labels[], float feaList[], int len) {
    float min = 100000000000000.0;
    float minSp = min;
    float minVar = min;
    for (int i = 0; i < len; ++i) {
        float var = dataVariance(labels, feaList, len, spList[i]);
        if (var < minVar) {
            minVar = var;
            minSp = spList[i];
        }
    }
    tuple_p ret = (tuple_p)malloc(sizeof(tuple));
    ret->first = minSp;
    ret->second = minVar;
    return ret;
}

tuple_p test_tuple(int a, int b) {
    tuple_p ret = (tuple_p)malloc(sizeof(tuple));
    ret->first = a;
    ret->second = b;
    return ret;
}

void free_tuple(tuple_p t) {
    free(t);
}

float get_tuple(tuple_p t) {
    return t->first;
}


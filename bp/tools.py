#coding:utf-8

import sys
import numpy as np

EPSLON = 1e-6

def softmax(x):
    if x.shape[1] != 1:
        print >> stderr, "shape error! x.shape=(%d,%d)" % (x.shape[0], x.shape[1])
        sys.exit(-1)
    one = np.matrix(np.ones((x.shape[0], 1), dtype=np.float64))
    try:
        ret = np.exp(x) / (one.T * np.exp(x) + EPSLON)
    except RuntimeWarning:
        print x
        print sys.exc_info()
    return ret
    # return np.exp(x) / (one.T * np.exp(x) + EPSLON)

def tanh_(x):
    # print x
    # return np.multiply(1 - np.tanh(x), 1 + np.tanh(x))
    # return np.multiply(1 - x, 1 + x)
    try:
        ret = np.multiply(1 - x, 1 + x)
    except RuntimeWarning:
        print x
        print sys.exc_info()
    return ret

def odot(x, y):
    # return np.multiply(x, y)
    try:
        ret = np.multiply(x, y)
    except RuntimeWarning:
        print x
        print sys.exc_info()
    return ret

def odot3(x, y, z):
    return np.multiply(np.multiply(x, y), z)

def exp(x):
    x[x > 30] = 30
    x[x < -30] = -30
    return np.exp(x)


def sigmoid(x):
    # print x.reshape(100,1)
    # return 1. / (1. + np.exp(-x))
    # return 1. / (1. + exp(-x))
    try:
        ret = 1. / (1. + np.exp(-x))
    except RuntimeWarning:
        print x
        print sys.exc_info()
    return ret 

def sigmoid_(x):
    try:
        ret = np.multiply(x, 1. - x)
    except RuntimeWarning:
        print x
        print sys.exc_info()
    return ret
    #return np.multiply(x, 1. - x)



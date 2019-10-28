import sys
import time
import numpy as np

import copy

from ctypes import *

# lib = CDLL('../lib/util.so')
lib = CDLL('../../lib/libutil.so')

# import libutil


class StructPointer(Structure):
    _fields_ = [("first", c_float), ("second", c_float)]

pairList = []
feaList = []
# for i in xrange(10000 + 1):
for i in xrange(43272 + 1):
    pairList.append( (i, i) )
    feaList.append(i)


minVar, minSp = np.inf, np.inf

spList = []
for i in range(1, len(feaList)):
    if feaList[i - 1] + 1e-7 < feaList[i]:
        spList.append(0.5 * (feaList[i - 1] + feaList[i]))


start = time.time()


'''
minSp, minVar = libutil.findLeastVar(spList, pairList)

print "run", len(spList), time.time() - start
print minSp, minVar
'''

import var

minSp, minVar = var.findLeastVar(spList, feaList, feaList)

print "run", len(spList), time.time() - start
print minSp, minVar

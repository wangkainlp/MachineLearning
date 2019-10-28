#coding:utf-8

def variance(numList):
    if len(numList) <= 1:
        return 0.
    s = 0.
    for i in numList:
        s += i
    avg = 1.0 * s / len(numList)
    # varSum = sum(map(lambda i : (i - avg) ** 2, numList))
    varSum = 0.
    for x in numList:
        varSum += (x - avg) ** 2
    return varSum

def dataVariance(feaLabelPair, sp):
    leftList, rightList = [], []
    for pair in feaLabelPair:
        if pair[0] <= sp:
            leftList.append(pair[1])
        else:
            rightList.append(pair[1])

    return variance(leftList) + variance(rightList)

def tests(numList):
    size = len(numList)
    if size <= 0:
        return 0.
    s = 0.
    for i in numList:
        s += i
    return -1 * (s ** 2) / size


def dataVarianceS(feaLabelPair, sp):
    leftList, rightList = [], []
    var = 0.0
    lsum = 0.0
    rsum = 0.0
    lsize = 0
    rsize = 0
    for pair in feaLabelPair:
        var += pair[1] ** 2
        if pair[0] <= sp:
            lsize += 1
            lsum += pair[1]
        else:
            rsize += 1
            rsum += pair[1]
    # return var + tests(leftList) + tests(rightList)
    return var + (-1 * (lsum ** 2) / lsize) + (-1 * (rsum ** 2) / rsize)

def dataVarianceS1(feaLabelPair, sp):
    lsum = 0.0
    rsum = 0.0
    lsize = 0
    rsize = 0
    for pair in feaLabelPair:
        if pair[0] <= sp:
            lsize += 1
            lsum += pair[1]
        else:
            rsize += 1
            rsum += pair[1]
    # return (-1 * (lsum ** 2) / lsize) + (-1 * (rsum ** 2) / rsize)
    return (-1 * (lsum  / lsize * lsum)) + (-1 * (rsum  / rsize * rsum))




def findLeastVar(spList, pairList):
    minSp = float('inf')
    minVar = float('inf')
    for sp in spList:
        var = dataVarianceS1(pairList, sp)
        if var < minVar:
            minVar = var
            minSp = sp
    return minSp, minVar

# def findLeastVar(spList, pairList):
#     minSp = float('inf')
#     minVar = float('inf')
#     for sp in spList:
#         # var = dataVariance(pairList, sp)
#         var = dataVarianceS(pairList, sp)
#         if var < minVar:
#             minVar = var
#             minSp = sp
#     return minSp, minVar




import random
import time
def findLeastVarS(spList, pairList):
    time.sleep(random.random() * 10)
    minSp = float('-inf')
    minVar = float('-inf')
    for sp in spList:
        # var = dataVariance(pairList, sp)

        leftList, rightList = [], []
        for pair in pairList:
            if pair[0] <= sp:
                leftList.append(pair[1])
            else:
                rightList.append(pair[1])

        leftVar = 0.
        rightVar = 0.
        if len(leftList) > 1:
            s = 0.
            for i in leftList:
                s += i
            avg = 1.0 * s / len(leftList)
            # varSum = sum(map(lambda i : (i - avg) ** 2, numList))
            for x in leftList:
                leftVar += (x - avg) ** 2

        if len(rightList) > 1:
            s = 0.
            for i in rightList:
                s += i
            avg = 1.0 * s / len(rightList)
            varSum = 0.
            for x in rightList:
                rightVar += (x - avg) ** 2
        var = leftVar + rightVar

        if var < minVar:
            minVar = var
            minSp = sp
    return minSp, minVar



def test_in():
    for i in range(10000):
        i ** 2

import time
def test():
    time.sleep(3)
    test_in()
    return float('-inf')

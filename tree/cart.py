#coding:utf-8
import sys
from tree_node import TreeNode 
import numpy as np

def variance(numList):
    if len(numList) <= 1:
        return 0.
    avg = 1.0 * sum(numList) / len(numList)
    varSum = sum(map(lambda i : (i - avg) ** 2, numList))
    return varSum

def dataVariance(data, fIdx, sp):
    feaLabelPair = [ (it[fIdx], it[-1]) for it in data ]
    leftList, rightList = [], []
    for pair in feaLabelPair:
        if pair[0] <= sp:
            leftList.append(pair[1])
        else:
            rightList.append(pair[1])

    return variance(leftList) + variance(rightList)


def findBestSpFea(data, fIdx):
    feaList = []

    for it in data:
        feaList.append(it[fIdx])
    feaSet = set(feaList)
    uniFeaList = list(feaSet)
    uniFeaList.sort()

    minVar, minSp = np.inf, np.inf

    if len(uniFeaList) <= 1:
        return minSp, minVar

    spList = []
    for i in range(1, len(uniFeaList), 1):
        spList.append( (uniFeaList[i - 1] + uniFeaList[i]) / 2.0 )

    for sp in spList:
        var = dataVariance(data, fIdx, sp)
        if var < minVar:
            minVar = var
            minSp = sp

    return minSp, minVar

def findBestSp(data, feaSize):
    curFeaList = []
    minFea, minSp, minVar = np.inf, np.inf, np.inf
    for fIdx in range(feaSize):
        sp, var = findBestSpFea(data, fIdx)
        if var < minVar:
            minFea = fIdx
            minVar = var
            minSp = sp
    return minFea, minSp, minVar

# 节点覆盖的最小数据量
minCoverNum = 3
maxDepth = 5
maxLeaves = 30

def cart_loop(data, root, feaSize):
    newNode = TreeNode(root, None, None)
    newNode.size = len(data)
    newNode.value = 1.0 * sum(map(lambda x : x[-1], data)) / len(data)

    # 节点覆盖的最小数据量
    if len(data) <= minCoverNum:
        newNode.itemIds = map(lambda x : x[-2], data)
        return newNode

    minFea, minSp, minVar = findBestSp(data, feaSize)

    if minFea == np.inf:
        newNode.itemIds = map(lambda x : x[-2], data)
        return newNode

    newNode.depth = root.depth + 1
    if newNode.depth >= maxDepth:
        newNode.itemIds = map(lambda x : x[-2], data)
        return newNode

    newNode.idx = minFea
    newNode.split = minSp

    print minFea, minSp, newNode.depth

    leftData = []
    rightData = []

    for it in data:
        if it[minFea] <= minSp:
            leftData.append(it)
        else:
            rightData.append(it)

    newNode.left = cart_loop(leftData, newNode, feaSize)
    newNode.right = cart_loop(rightData, newNode, feaSize)

    return newNode


def cart_main(data):
    feaSize = len(data[0]) - 2
    itemSize = len(data)
    print "feaSize:%d dataSize:%d" % (feaSize, itemSize)

    tmpRoot = TreeNode(None, None, None)
    tmpRoot.depth = 0

    head = cart_loop(data, tmpRoot, feaSize)
    head.father = None

    return head

   
if __name__ == '__main__':
    pass


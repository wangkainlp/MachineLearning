#coding:utf-8
import sys
import copy
import random
import cart_gb as cart
import time
from tree_node import *
import tree

from TreeSVG import *

def getLeaves(root):
    leaves = []
    nodes = [root]
    while len(nodes) > 0:
        newNodes = []
        for it in nodes:
            if not it.left and not it.right:
                leaves.append(it)
            if it.left:
                newNodes.append(it.left)
            if it.right:
                newNodes.append(it.right)
        nodes = newNodes
    return leaves

def sampleWithoutReplace(low, high, numSize):
    numSet = set()
    while len(numSet) < numSize:
        num = random.randint(low, high)
        numSet.add(num)
    return list(numSet)


def dataSample(data, sampleSize):
    cData = []
    idList = sampleWithoutReplace(0, len(data) - 1, sampleSize)
    for i in idList:
        cData.append(data[i])
    return cData

def featureSample(dimSize, feaSize):
    return sampleWithoutReplace(0, dimSize - 1, feaSize)

def gbPred(treeList, feaList):
    score = 0.0
    for tree in treeList:
        score += cart.pred(tree, feaList) * Shrink
    return score

def copy_tree(tree_list, idx, root):
    copy_elem(tree_list, idx, root) 
    if (2 * idx + 1) < len(tree_list):
        if len(tree_list[2 * idx + 1]) > 0:
            left = TreeNode()
            root.left = left
            copy_tree(tree_list, 2 * idx + 1, left) 
    if 2 * idx + 2 < len(tree_list):
        if len(tree_list[2 * idx + 2]) > 0:
            right = TreeNode()
            root.right = right
            copy_tree(tree_list, 2 * idx + 2, right)

def copy_elem(tree_list, idx, tmp):
    if len(tree_list[idx]) > 0:
        tmp.split = tree_list[idx]['split']
        tmp.idx   = tree_list[idx]['fIdx']
        tmp.value = tree_list[idx]['value']
        tmp.depth = tree_list[idx]['depth']
        tmp.size  = tree_list[idx]['size']


Shrink = 0.6

def gb(data, M, sampleRate, feaRate):
    shrink = Shrink
    dataSize = len(data)
    dimSize = len(data[0]) - 2

    sampleSize = int(sampleRate * dataSize)
    print dataSize, sampleSize
    feaSize = int(feaRate * dimSize)
    print "sampleSize:%d feaSize:%d" % (sampleSize, feaSize)

    treeList = []
    eData = copy.deepcopy(data)
    for i in range(M):
        print '-' * 50
        print i
        # cData = dataSample(eData, sampleSize)
        # cFeaList = featureSample(dimSize, feaSize)
        # print "sample:", len(cData), len(cFeaList)
        # tree = cart.cart_main(cData, cFeaList)

        cDataSample = sampleWithoutReplace(0, dataSize - 1, sampleSize)
        cFeaSample  = sampleWithoutReplace(0, dimSize - 1,  feaSize) 
        print "sample:", len(cDataSample), len(cFeaSample) 


        data_flat = [ _j for _i in eData for _j in _i ]

        begin = time.time()

        tree_list = tree.testdata(data_flat, dataSize, len(data[0]), \
                                  cDataSample, cFeaSample)

        print i, "end :", time.time() - begin

        # continue
        # tree_list = tree.testdata(data, 15, 5)
        root = TreeNode()
        copy_tree(tree_list, 0, root)
                
        pain = TreeSVG(root)
        pain.draw()

        outSvg = open('test.svg', 'w')
        print >> outSvg, pain.pageHtml

        '''
        leaves = getLeaves(tree)
        for leaf in leaves:
            cost = 0.0
            c = 0.0
            for dataId in leaf.itemIds:
                fea = data[dataId]
                cost += 0.5 * (data[dataId][-1] - (gbPred(treeList, fea) + c)) ** 2
            leaf.value = c

        '''

        # treeList.append(tree)
        treeList.append(root)

        eData = copy.deepcopy(data)
        for it in eData:
            # y = cart.pred(tree, it)
            y = gbPred(treeList, it)
            # old = it[-1]
            error = it[-1] - y
            # it[-1] = shrink * error
            it[-1] = error

    return treeList

    '''
    print "-" * 50
    print "pred"
    for it in data:
        y_pred = gbPred(treeList, it)
        print it[-1], y_pred
    '''




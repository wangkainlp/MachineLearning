#coding:utf-8
import sys
import numpy as np
# from tree_node import TreeNode 
from tree_node import *
from ccp import *
import copy

import threading
import time
import multiprocessing

from lib import libutil

from ctypes import *
lib = CDLL('lib/util.so')

class StructPointer(Structure):
    _fields_ = [("first", c_float), ("second", c_float)]

import var

def findBestSpFeaP_1(feaSortList, feaList, labelList, fIdx):

    spList = []
    for i in range(1, len(feaSortList)):
        if feaSortList[i - 1] + 1e-7 < feaSortList[i]:
            spList.append(0.5 * (feaSortList[i - 1] + feaSortList[i]))

    minVar, minSp = np.inf, np.inf

    # 特征值只有一个
    if len(spList) <= 0:
        return fIdx, minSp, minVar
            
    # start = time.time()
    minSp, minVar = var.findLeastVar(spList, feaList, labelList)
    # print 'run end', fIdx, time.time() - start

    return fIdx, minSp, minVar


def findBestSpFeaP(id2SortId, dataIdList, tmpMap, feaData, fIdx):
    # dataIdList = map(lambda x: x[-2], data)
    # tmpMap = { it[-2] : idx for idx, it in enumerate(data) }
    # print "not lib begin:", fIdx, time.time()
    pairList = feaData
    _feaList = [float('-inf')] * 50000
    for idx in dataIdList:
        feaData[tmpMap[idx]][0]
        try:
            # _feaList[id2SortId[idx]] = data[tmpMap[idx]][fIdx]
            _feaList[id2SortId[idx]] = feaData[tmpMap[idx]][0]
        except IndexError, e:
            print >> sys.stderr, id2SortId[idx]
            print >> sys.stderr, sys.exc_info()
            print >> sys.stderr, 'index error'
            sys.exit(-1)

    feaList = []
    neg_inf = float('-inf')
    for it in _feaList:
        if it > neg_inf:
            feaList.append(it)


    '''
    uniFeaList = [feaList[0]]
    for i in feaList[1:]:
        if i != uniFeaList[-1]:
            uniFeaList.append(i)
    # print 'start 1', time.time()

    minVar, minSp = np.inf, np.inf

    # 特征值只有一个
    if len(uniFeaList) <= 1:
        return fIdx, minSp, minVar

    spList = map(lambda x, y: (x + y) / 2.0, uniFeaList[1:], uniFeaList[:-1])
    '''

    minVar, minSp = np.inf, np.inf
    spList = []
    for i in range(1, len(feaList)):
        if feaList[i - 1] + 1e-7 < feaList[i]:
            spList.append(0.5 * (feaList[i - 1] + feaList[i]))

    # 特征值只有一个
    if len(spList) <= 0:
        return fIdx, minSp, minVar
            

    # print "not lib end:", fIdx, time.time()

    # print 'start v', time.time()
    # print len(spList)

    # pairList = [ (it[fIdx], it[-1]) for it in data ]

    # print 'run start', fIdx, time.time()
    start = time.time()

    # minSp, minVar = libutil.findLeastVar(spList, pairList)
    minSp, minVar = var.findLeastVar(spList, feaList)

    # lib.findLeastVar.restype = POINTER(StructPointer)

    # lib.free_tuple.argtype = POINTER(StructPointer)
    # lib.free_tuple.restype = None

    # lib.free_tuple.argtype = POINTER(StructPointer)
    # lib.free_tuple.restype = None

    # feas = [it[0] for it in pairList]
    # labels = [ it[1] for it in pairList]
    # size = len(feas)

    # p = lib.findLeastVar((c_float * size)(* spList),
    #                    (c_float * size)(* labels),
    #                    (c_float * size)(* feaList),
    #                    c_int(size) )

    # minSp = p.contents.first
    # minVar = p.contents.second
    # lib.free_tuple(p)

    # minSp = minSp if minSp < (100000000000000.0 - 100) else  float('inf')
    # minVar = minVar if minVar < (100000000000000.0 - 100) else float('inf')

    
    # print "run", fIdx, len(dataIdList), len(spList), time.time() - start
    # print 'run end', fIdx, time.time()

    '''
    print 'run start', fIdx, time.time()
    for sp in spList:
        # var = libutil.dataVariance(pairList, sp)
        var = dataVariance(pairList, sp)
        if var < minVar:
            minVar = var
            minSp = sp
    print 'run end', fIdx, time.time()
    '''

    return fIdx, minSp, minVar



def findBestSpFea(data, fIdx): 
    id2SortId = id2Sortid[fIdx]
    # print 'start', time.time()
    dataIdList = map(lambda x: x[-2], data)
    tmpMap = { it[-2] : idx for idx, it in enumerate(data) }
    _feaList = [float('-inf')] * 50000
    for idx in dataIdList:
        data[tmpMap[idx]]
        try:
            _feaList[id2SortId[idx]]
            _feaList[id2SortId[idx]] = data[tmpMap[idx]][fIdx]
        except IndexError, e:
            print >> sys.stderr, id2SortId[idx]
            print >> sys.stderr, sys.exc_info()
            sys.exit(-1)

    feaList = []
    neg_inf = float('-inf')
    for it in _feaList:
        if it > neg_inf:
            feaList.append(it)


    uniFeaList = [feaList[0]]
    for i in feaList[1:]:
        if i != uniFeaList[-1]:
            uniFeaList.append(i)
    # print 'start 1', time.time()

    minVar, minSp = np.inf, np.inf

    # 特征值只有一个
    if len(uniFeaList) <= 1:
        return minSp, minVar

    '''
    spList = []
    for i in range(1, len(uniFeaList), 1):
        spList.append( (uniFeaList[i - 1] + uniFeaList[i]) / 2.0 )
    '''
    spList = map(lambda x, y: (x + y) / 2.0, uniFeaList[1:], uniFeaList[:-1])

    # print 'start v', time.time()
    # print len(spList)

    pairList = [ (it[fIdx], it[-1]) for it in data ]

    # print "lib begin:", fIdx, time.time()
    minSp, minVar = libutil.findLeastVar(spList, pairList)
    # print "lib end:", fIdx, time.time()

    # for sp in spList:
    #     # var = dataVariance(data, fIdx, sp)
    #     var = dataVariance(pairList, sp)
    #     # var = libutil.dataVariance(pairList, sp)
    #     if var < minVar:
    #         minVar = var
    #         minSp = sp

    return minSp, minVar

def findBestSp(data, dataIds, feaList):

    begin = time.time()
    print "begin", begin

    spList = []
    M = 1
    feaIdxList = feaList
    pool = multiprocessing.Pool(M)
    tasks = []
    for fIdx in feaIdxList:

        _id2Sortid = id2Sortid[fIdx]
        _feaSortList = [ None ] * len(data)

        for id in dataIds:
            _feaSortList[_id2Sortid[id]] = data[id][fIdx]
        feaSortList = [ fea for fea in _feaSortList if fea is not None ]
        feaList = [ it[fIdx] for it in data ]
        labelList = [ it[-1] for it in data ]

        '''
        _id2Sortid = id2Sortid[fIdx]
        _feaSortList = [ None ] * len(data)
        _feaSortCntList = [ 0 ] * len(data)
        for id in dataIds:
            if mask[id] > 0:
                _feaSortList[ sortid[id] ] = data[id][fIdx]
                _feaSortCntList[ sortid[id] ] = mask[id]

        for i in range(len(_feaSortList)):
            for 
            feaSortList.append(
        '''




        task = pool.apply_async(findBestSpFeaP_1, (feaSortList, feaList, labelList, fIdx))
        # task = pool.apply_async(func, args=(i, nums))
        tasks.append(task)

    pool.close()
    pool.join()

    for i in tasks:
        spList.append(i.get())

 
    minFea, minSp, minVar = np.inf, np.inf, np.inf
    # for fIdx in range(feaSize):
    for fIdx, sp, var in spList:
        if var < minVar:
            minFea = fIdx
            minVar = var
            minSp = sp
    '''
    if minVar < np.inf:
        tmpVar = 0.0
        for it in data:
            tmpVar += it[-1] ** 2
        minVar = 
    '''


    print "end", time.time() - begin
    return minFea, minSp, minVar


def cart_loop(data, dataIds, root, feaList):

   
    newNode = TreeNode(root, None, None)
    newNode.size = len(dataIds)
    newNode.value = 1.0 * sum(map(lambda id : data[id][-1], dataIds)) / len(dataIds)
    # newNode.size = len(data)
    # newNode.value = 1.0 * sum(map(lambda x : x[-1], data)) / len(data)

    # 节点覆盖的最小数据量
    if len(data) <= minCoverNum:
        newNode.itemIds = map(lambda x : x[-2], data)
        return newNode

    minFea, minSp, minVar = findBestSp(data, dataIds, feaList)

    if minFea == np.inf:
        newNode.itemIds = map(lambda x : x[-2], data)
        return newNode

    newNode.depth = root.depth + 1
    if newNode.depth >= maxDepth:
        newNode.itemIds = map(lambda x : x[-2], data)
        return newNode

    newNode.idx = minFea
    newNode.split = minSp

    # print "node:", minFea, minSp, newNode.depth

    leftData = []
    leftDataIds = []
    rightData = []
    rightDataIds = []

    # for it, id in zip(data, dataIds):
    for id in dataIds:
        if data[id][minFea] <= minSp:
            # leftData.append(it)
            leftDataIds.append(id)
        else:
            # rightData.append(it)
            rightDataIds.append(id)

    # 不能继续分
    if len(leftDataIds) <= 0 or len(rightDataIds) <= 0:
        print minFea, minSp, minVar

    # newNode.left = cart_loop(leftData, newNode, leftDataIds, feaSize)
    # newNode.right = cart_loop(rightData, newNode, rightDataIds, feaSize)

    newNode.left  = cart_loop(data, leftDataIds,  newNode, feaList)
    newNode.right = cart_loop(data, rightDataIds, newNode, feaList)

    return newNode

def pred(tree, fea):
    curNode = tree
    while curNode.left and curNode.right:
        if fea[curNode.idx] <= curNode.split:
            curNode = curNode.left
        else:
            curNode = curNode.right

    if not curNode.left and not curNode.right:
        return curNode.value
    else:
        print >> sys.stderr, "node error!"
        print >> sys.stderr, "id:%d idx:%d" % (curNode.id, curNode.idx)
        return None

    '''
    if not tree.left and not tree.right:
        return tree.value

    if fea[tree.idx] <= tree.split:
        return pred(tree.left, fea)
    else:
        return pred(tree.right, fea)
    '''

# 节点覆盖的最小数据量
# minCoverNum = 3
minCoverNum = 50
maxDepth = 5
maxLeaves = 10
feaSortMap = {}
id2Sortid = {}
sortid2Id= {}
Data = []

def cart_main(data, feaList):
    begin = time.time()
    feaSize = len(feaList)
    itemSize = len(data)
    print "feaSize:%d dataSize:%d" % (feaSize, itemSize)


    tmpRoot = TreeNode(None, None, None)
    tmpRoot.depth = 0

    #pre_sort
    start = time.time()
    for i in feaList:
        # _feaList = [ (it[-2], it[i]) for it in data ]
        _feaList = [ (id, it[i]) for id, it in enumerate(data) ]
        feaSortMap[i] = sorted(_feaList, key=lambda x: x[1])
        id2Sortid[i] = { it[0] : sortId for sortId, it in enumerate(feaSortMap[i]) }
        sortid2Id[i] = { sortId : it[0] for sortId, it in enumerate(feaSortMap[i]) }
        
    print 'sort', time.time() - start

    dataIds = range(len(data))

    root = cart_loop(data, dataIds, tmpRoot, feaList)
    root.father = None

    # id
    level = [root]
    nodeId = 0
    while len(level) > 0:
        newLevel = []
        for node in level:
            if node.left:
                newLevel.append(node.left)
            if node.right:
                newLevel.append(node.right)
            #visit
            node.id = nodeId
            nodeId += 1
            # print nodeId
        level = newLevel

    print "end tree:", time.time() - begin

    return root

   
if __name__ == '__main__':
        pass


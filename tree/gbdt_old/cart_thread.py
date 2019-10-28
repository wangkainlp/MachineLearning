#coding:utf-8
import sys
import numpy as np
# from tree_node import TreeNode 
from tree_node import *
from ccp import *
import copy

import threading
import time

from lib import libutil

lock = threading.Lock()

class MyThread(threading.Thread):
    def __init__(self, func, args=()):
        super(MyThread, self).__init__()
        self.func = func
        self.args = args

    def run(self):
        # print "run start"
        self.result = self.func(*self.args)
        # print "run end"

    def get_result(self):
        # threading.Thread.join(self)
        try:
            return self.result
        except Exception:
            return None

def variance(numList):
    if len(numList) <= 1:
        return 0.
    avg = 1.0 * sum(numList) / len(numList)
    # varSum = sum(map(lambda i : (i - avg) ** 2, numList))
    varSum = reduce(lambda y, x : y + (x - avg) ** 2, numList)
    return varSum

'''
def dataVariance(data, fIdx, sp):
    feaLabelPair = [ (it[fIdx], it[-1]) for it in data ]
    leftList, rightList = [], []
    for pair in feaLabelPair:
        if pair[0] <= sp:
            leftList.append(pair[1])
        else:
            rightList.append(pair[1])

    return variance(leftList) + variance(rightList)
'''

def dataVariance(feaLabelPair, sp):
    leftList, rightList = [], []
    for pair in feaLabelPair:
        if pair[0] <= sp:
            leftList.append(pair[1])
        else:
            rightList.append(pair[1])

    return variance(leftList) + variance(rightList)

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


    uniFeaList = [feaList[0]]
    for i in feaList[1:]:
        if i != uniFeaList[-1]:
            uniFeaList.append(i)
    # print 'start 1', time.time()

    minVar, minSp = np.inf, np.inf

    # 特征值只有一个
    if len(uniFeaList) <= 1:
        return minSp, minVar

    spList = map(lambda x, y: (x + y) / 2.0, uniFeaList[1:], uniFeaList[:-1])
    # print "not lib end:", fIdx, time.time()

    # print 'start v', time.time()
    # print len(spList)

    # pairList = [ (it[fIdx], it[-1]) for it in data ]

    '''
    print 'run start', fIdx, time.time()
    minSp, minVar = libutil.findLeastVar(spList, pairList)
    print 'run end', fIdx, time.time()

    '''
    print 'run start', fIdx, time.time()
    for sp in spList:
        # var = libutil.dataVariance(pairList, sp)
        var = dataVariance(pairList, sp)
        if var < minVar:
            minVar = var
            minSp = sp
    print 'run end', fIdx, time.time()

    return minSp, minVar



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

    '''
    feaList = [it[fIdx] for it in data]
    uniFeaList = list(set(feaList))
    uniFeaList.sort()
    feaList.sort()
    uniFeaList = [feaList[0]]
    for i in feaList[1:]:
        if i != uniFeaList[-1]:
            uniFeaList.append(i)
    '''


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
    '''

    varSpList = []

    pairList = [ (it[fIdx], it[-1]) for it in data ]
    M = 10
    groups = (len(spList) + M - 1) / M
    for g in range(groups):
        tasks = []
        p = g * M
        q = min(len(spList), (g + 1) * M)
        mSpList = spList[p:q]
        for sp in mSpList:
            # task = MyThread(dataVariance, (data, fIdx, sp))
            task = MyThread(dataVariance, (pairList, sp))
            tasks.append(task)
            task.start()

        for task in tasks:
            task.join()
            varSpList.append((task.get_result(), task.args[1]))
        
    for it in varSpList:
        var, sp = it
        if var < minVar:
            minVar = var
            minSp = sp
    '''
    # print 'end', time.time()

    return minSp, minVar

def findBestSp(data, feaSize):
    curFeaList = []

    print "begin", time.time()

    '''
    spList = []
    tasks = []
    for fIdx in range(feaSize):
        print "begin th", time.time()
        id2SortId = id2Sortid[fIdx]
        task = MyThread(findBestSpFea, (id2SortId, data, fIdx))
        tasks.append(task)
        task.start()
        print "end th", time.time()

    print "tasks:", len(tasks)
    for task in tasks:
        task.join()
        # spList.append(task.get_result())
        spList.append((task.get_result()))
    '''

    spList = []
    M = 10
    groups = []
    feaIdxList = range(feaSize)
    # feaGroups.extend(feaIdxList[0:M])
    # del feaIdxList[0 : M]

    tasks = []
    endFlag = 0

    while len(tasks) > 0 or len(feaIdxList) > 0:
        if len(tasks) < M:
            addNum = min(M - len(tasks), len(feaIdxList))
            addList = feaIdxList[0 : addNum]
            print "add num", addNum
            for fIdx in addList:
                print "begin th", fIdx, time.time()

                id2SortId = copy.deepcopy(id2Sortid[fIdx]) 
                dataIdList = map(lambda x: x[-2], data)
                tmpMap = { it[-2] : idx for idx, it in enumerate(data) }
                pairList = [ (it[fIdx], it[-1]) for it in data ]

                task = MyThread(findBestSpFeaP, (id2SortId, dataIdList, tmpMap, pairList, fIdx))
                tasks.append(task)
                task.start()
                print "start th", fIdx, time.time()
            del feaIdxList[0 : addNum]

        for i in range(len(tasks))[::-1]:
            task = tasks[i]
            if task.isAlive() == False:
                spList.append((task.get_result()))
                print "end th", task.args[-1], time.time()
                del tasks[i]
            time.sleep(0.1)

        time.sleep(2)





    ''' 
    spList = []
    M = 10
    groups = (feaSize + M - 1) / M
    fList = range(feaSize)
    for g in range(groups):
        tasks = []
        p = g * M
        q = min(len(fList), (g + 1) * M)
        mfList = fList[p:q]
        for fIdx in mfList:
            print "begin th", fIdx, time.time()

            id2SortId = copy.deepcopy(id2Sortid[fIdx]) 
            dataIdList = map(lambda x: x[-2], data)
            tmpMap = { it[-2] : idx for idx, it in enumerate(data) }
            pairList = [ (it[fIdx], it[-1]) for it in data ]
            task = MyThread(findBestSpFeaP, (id2SortId, dataIdList, tmpMap, pairList, fIdx))
            tasks.append(task)
            task.start()
            print "start th", fIdx, time.time()

        for task in tasks:
            task.join()
            spList.append((task.get_result()))
            print "end th", task.args[-1], time.time()
    '''
 
    minFea, minSp, minVar = np.inf, np.inf, np.inf
    for fIdx in range(feaSize):
        sp, var = spList[fIdx]
        if var < minVar:
            minFea = fIdx
            minVar = var
            minSp = sp

    '''
    minFea, minSp, minVar = np.inf, np.inf, np.inf
    for fIdx in range(feaSize):
        sp, var = findBestSpFea(data, fIdx)
        if var < minVar:
            minFea = fIdx
            minVar = var
            minSp = sp
    '''

    print "end", time.time()
    return minFea, minSp, minVar


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

    # print "node:", minFea, minSp, newNode.depth

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
Data = []

def cart_main(data):
    feaSize = len(data[0]) - 2
    itemSize = len(data)
    print "feaSize:%d dataSize:%d" % (feaSize, itemSize)


    tmpRoot = TreeNode(None, None, None)
    tmpRoot.depth = 0

    #pre_sort
    for i in range(feaSize):
        feaList = [ (it[-2], it[i]) for it in data ]
        feaSortMap[i] = sorted(feaList, key=lambda x: x[1])
        id2Sortid[i] = { it[0] : sortId for sortId, it in enumerate(feaSortMap[i]) }


    root = cart_loop(data, tmpRoot, feaSize)
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

    return root

   
if __name__ == '__main__':
        pass


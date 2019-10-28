#coding:utf-8
import sys
import copy
import random
import time
import math
import gc

from   tree_node import *
import tree
# import cart

# from TreeSVG import *

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

def gbPred(treeList, feaList):
    score = 0.0
    '''
    for tree in treeList:
        score += pred(tree, feaList)
    return score
    '''
    i = 0
    size = len(treeList)
    while i < size:
        score += pred(treeList[i], feaList)
        i += 1
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
        tmp.id = 0
        tmp.left = None
        tmp.right = None

def clip(val):
    if val<0.00001:
        val = 0.00001
    elif val>0.99999:
        val = 0.99999
    return val

def transform(val):
    ret = 0.0
    if val > 0: 
        ret = 1.0 / (1.0 + math.exp(-val))
    else:
        ex = math.exp(val)
        ret = ex / (1.0 + ex)
    return clip(ret)


# def boost(data, M, sampleRate, feaRate):
def boost(data, conf):
    shrink = conf['shrink']
    dataSize = conf['data_size']
    dimSize  = conf['dim_size']

    sampleSize = int(conf['sample_rate'] * dataSize)
    feaSize    = int(conf['feature_rate'] * dimSize)
    print "dataSize:%d sampleSize:%d" % (dataSize, sampleSize)
    print "sampleSize:%d feaSize:%d" % (sampleSize, feaSize)

    data_flat = [ _j for _i in data for _j in _i ]

    rowRand = []
    colRand = []
    i = 0
    while i < conf['tree_size']:
        i += 1
        rowRand.append( sampleWithoutReplace(0, dataSize - 1, sampleSize) )
        colRand.append( sampleWithoutReplace(0, dimSize - 1,  feaSize) )
        

    treeList = []
    labels = [0.5] * dataSize
    # for i in range(conf['tree_size']):
    i = 0
    while i < conf['tree_size']:
        print '-' * 50
        print 'tree:', i

        cDataSample = rowRand[i] # range(dataSize) # sampleWithoutReplace(0, dataSize - 1, sampleSize)
        cFeaSample  = colRand[i] # range(dimSize)  # sampleWithoutReplace(0, dimSize - 1,  feaSize) 
        print "sample:", len(cDataSample), len(cFeaSample)

        begin = time.time()

        tree_list = tree.xgboost(data_flat, conf, labels, cDataSample, cFeaSample)
        print tree_list

        print i, "end :", time.time() - begin

        # continue
        # tree_list = tree.testdata(data, 15, 5)
        root = TreeNode()
        copy_tree(tree_list, 0, root)
                
        '''
        pain = TreeSVG(root)
        pain.draw()


        outSvg = open('test.svg', 'w')
        # outSvg.truncate()
        # print >> sys.stderr, pain.pageHtml
        print >> outSvg, pain.pageHtml
        outSvg.close
        '''

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
        del tree_list
        gc.collect()

        
        labels = []
        # for it in data:
        tmp_size = len(data)
        tmp_i = 0;
        while tmp_i < tmp_size:
            it = data[tmp_i]
            tmp_i += 1
            _y = gbPred(treeList, it)
            prob = transform(_y)
            # prob = 1.0 / (1.0 + math.exp(-_y))
            # labels.append(_y)
            labels.append(prob)
            print it[-1], _y

        i += 1

    return treeList

    '''
    print "-" * 50
    print "pred"
    for it in data:
        y_pred = gbPred(treeList, it)
        print it[-1], y_pred
    '''

def gb(data, conf):
    begin = time.time()

    data_flat = [ _j for _i in data for _j in _i ]

    trees = tree.gbdt(data_flat, conf);

    treeList = []
    for i in range(len(trees)):
        gbtree = trees[i]
        root = TreeNode()
        copy_tree(gbtree, 0, root)
        treeList.append(root)

        '''
        pain = TreeSVG(root)
        pain.draw()
        outSvg = open('test.svg', 'w')
        # outSvg.truncate()
        # print >> sys.stderr, pain.pageHtml
        print >> outSvg, pain.pageHtml
        outSvg.close
        '''
 

    return treeList
    '''
    labels = []
    for it in data:
        _y = xgboost.gbPred(treeList, it)
        prob = 1.0 / (1.0 + math.exp(-1.0 * _y))
        # labels.append(_y)
        labels.append(prob)
        print it, ':', _y, ',', prob

        pain = TreeSVG(root)
        pain.draw()

        outSvg = open('test.svg', 'w')
        # outSvg.truncate()
        print >> outSvg, pain.pageHtml
        outSvg.close

    print "run ", time.time() - begin
    '''


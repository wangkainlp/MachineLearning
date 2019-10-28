#coding:utf-8

import sys
import time
import math
sys.path.append('..')
sys.path.append('../..')



'''
import spam
# status = spam.system("ls -l")

# print status
'''

import tree
import xgboost
from tree_node import *
from TreeSVG import *


'''
# data = range(1000000)
data = range(20, -1, -1)

data = [ 1, 2, 3, 1, \
         2, 3, 5, 0, \
         5, 8, 1, 0, \
         2, 1, 3, 1, \
         3, 5, 2, 1 ]

data = [0, 0, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 1, 0, 1, 1,
        0, 1, 1, 0, 1,
        0, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 1, 0,
        1, 1, 1, 1, 1,
        1, 0, 1, 2, 1,
        1, 0, 1, 2, 1,
        2, 0, 1, 2, 1,
        2, 0, 1, 1, 1,
        2, 1, 0, 1, 1,
        2, 1, 0, 2, 1,
        2, 0, 0, 0, 0 ]

for i in range(15):
    for j in range(5):
        print "%d\t" % (data[ i * 5 + j]),
    print ''

'''



data = [[1, -5, 0], 
        [2,  5, 0], 
        [3, -2, 1], 
        [1,  2, 1], 
        [2,  0, 1], 
        [6, -5, 1], 
        [7,  5, 1], 
        [6, -2, 0], 
        [7,  2, 0], 
        [6,  0, 1], 
        [8, -5, 1], 
        [9,  5, 1], 
        [10,-2, 0], 
        [8,  2, 0], 
        [9,  0, 1]]

data_flat = [ j for i in data for j in i ]


for i in data:
    for j in i:
        print "%d\t" % (j),
    print ''

conf = {
        'row_width'      : 15,      \
        'col_width'      : 3,       \
        'data_size'      : 15,      \
        'dim_size'       : 2,       \
        'col_sample'     : 1.0,   \
        'row_sample'     : 1.0,   \
        'tree_size'      : 2,     \
        'shrink'         : 0.1,     \
        'max_depth'      : 4,       \
        'min_cover_num'  : 1,       \
        'lambda'         : 1,   \
        'min_gain'       : 0.0, \
        'seed'           : 1,  \
        'None'           : 0 }

# tree.testdata(data, 15, 5, range(15), range(4))
# tree.xgboost(data, conf, [0.5] * 15, range(15), range(2))

def test_xgboost():
    begin = time.time()
    labels = [0.5] * 15
    treeList = []
    for i in range(conf['tree_size']):
        gbtree = tree.xgboost(data_flat, conf, labels, range(15), range(2))

        root = TreeNode()
        xgboost.copy_tree(gbtree, 0, root)
        treeList.append(root)

        labels = []
        for it in data:
            _y = xgboost.gbPred(treeList, it)
            prob = 1.0 / (1.0 + math.exp(-1.0 * _y))
            # labels.append(_y)
            labels.append(prob)
            print it, ':', _y, ',', prob

    print "run ", time.time() - begin

def test_xgboost_gbm():
    begin = time.time()

    trees = gbtree = tree.xgboost_gbm(data_flat, conf);

    treeList = []
    for i in range(len(trees)):
        gbtree = trees[i]
        root = TreeNode()
        xgboost.copy_tree(gbtree, 0, root)
        treeList.append(root)

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
    root = TreeNode()
    xgboost.copy_tree(gbtree, 0, root)
    treeList.append(root)

    labels = []
    for it in data:
        _y = xgboost.gbPred(treeList, it)
        prob = 1.0 / (1.0 + math.exp(-1.0 * _y))
        # labels.append(_y)
        labels.append(prob)
        print it, ':', _y, ',', prob

    print "run ", time.time() - begin
    '''





if __name__ == '__main__':
    # test_xgboost()

    test_xgboost_gbm()

    '''
    import noddy

    n = noddy.Noddy()
    n.first = "a"

    print n.test()
    '''



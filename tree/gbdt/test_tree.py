#coding:utf-8
import sys
from load_data import *
sys.path.append('..')

from cart import *
import gbdt
import random as rand
from TreeSVG import TreeSVG
import time

conf = {}
conf['rate'] = 0.8

if __name__ == '__main__':

    irisDataFile = sys.argv[1]

    '''
    irisData, idx2label, label2idx = load_iris_data(irisDataFile)
    '''
    irisData = load_ec_data(irisDataFile)

    # print idx2label, label2idx

    rand.seed(int(time.time()))
    rand.shuffle(irisData)

    trainSize = int(len(irisData) * conf['rate'])
    irisTrainData = irisData[:trainSize]
    irisTestData = irisData[trainSize:]

    print "train size: %d test size: %d" % (len(irisTrainData), len(irisTestData))
    
    # 数据编号
    for i in range(len(irisTrainData)):
        irisTrainData[i].insert(-1, i)
        irisTrainData[i][-1] = 1 if irisTrainData[i][-1] == 1 else 0

    for i in range(len(irisTestData)):
        irisTestData[i][-1] = 1 if irisTestData[i][-1] == 1 else 0


    # 训练：树的棵树、样本采样率、特征采样率
    gbTrees = gbdt.gb(irisTrainData, 5, 0.8, 0.6)

    irisTestData = irisTrainData

    # 统计准确率
    accuCnt = 0
    for it in irisTestData:
        score = gbdt.gbPred(gbTrees, it)
        label = 1 if score > 0.5 else 0
        # label = score
        if abs(label - it[-1]) < 0.01:
            accuCnt += 1
        # print "%0.5f" %(score), label, it[-1]
    print "acc:%d, sum:%d, acc_rate:%f" \
          % (accuCnt, len(irisTestData), 1.0 * accuCnt / len(irisTestData))

    '''
    treeList = cart_main(irisTrainData)
    print "tree size:", len(treeList)

    for i, tree in enumerate(treeList):
        svg = TreeSVG(tree)
        svg.draw()
        outSvg = open('test-cart-%d.svg' % (i), 'w')
        print >> outSvg, svg.pageHtml
    '''




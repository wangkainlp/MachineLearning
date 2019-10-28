#coding:utf-8
import sys
from load_data import *
# sys.path.append('..')

import time
import random as rand
import gbm
# from TreeSVG import TreeSVG

import eval

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
    # rand.shuffle(irisData)

    trainSize = int(len(irisData) * conf['rate'])
    # irisTrainData = irisData[:trainSize]
    irisTrainData = irisData[:]
    irisTestData = irisData[trainSize:]

    print "train size: %d test size: %d" % (len(irisTrainData), len(irisTestData))
    
    # 数据编号
    for i in range(len(irisTrainData)):
        irisTrainData[i].insert(-1, i)
        irisTrainData[i][-1] = 1 if irisTrainData[i][-1] == 1 else 0

    for i in range(len(irisTestData)):
        irisTestData[i][-1] = 1 if irisTestData[i][-1] == 1 else 0

    
    conf = {
            'row_width'      : len(irisTrainData),    \
            'col_width'      : len(irisTrainData[0]), \
            'data_size'      : len(irisTrainData), \
            'dim_size'       : len(irisTrainData[0]) - 2, \
            'feature_rate'   : 0.8,   \
            'sample_rate'    : 0.8,   \
            'col_sample'     : 0.8,   \
            'row_sample'     : 0.8,   \
            'tree_size'      : 100,     \
            'shrink'         : 0.1,    \
            'max_depth'      : 12,       \
            'min_cover_num'  : 30,      \
            'min_gain'       : 0.01,    \
            'lambda'         : 0.001,   \
            'seed'           : 0,  \
            'None'           : 0 }

    # 训练：树的棵树、样本采样率、特征采样率
    gbTrees = gbm.gb(irisTrainData, conf)

    # 统计准确率
    accuCnt = 0
    irisTestData = irisTrainData
    # for it in irisTestData:
    
    # tmp_i = 0;
    # tmp_size = len(irisTestData)
    # while tmp_i < tmp_size:
    #     it = irisTestData[tmp_i]
    #     tmp_i += 1
    predList = []
    scoreList = []
    labelList = []
    for it in irisTestData:
        # score = gbm.gbPred(gbTrees, it)
        score = gbm.gbPred(gbTrees, it) * conf['shrink']
        # score = gbm.transform(score)
        label = 1 if score > 0.5 else 0
        # label = score
        if abs(label - it[-1]) < 0.01:
            accuCnt += 1
        scoreList.append(score)
        predList.append(label)
        labelList.append(it[-1])
        print "%0.5f\t%d\t%d" %(score, int(label), int(it[-1]))

    print "acc:%d, sum:%d, acc_rate:%f" \
          % (accuCnt, len(irisTestData), 1.0 * accuCnt / len(irisTestData))

    p, r, f = eval.PRF(predList, labelList)
    print 'Precise:%f\tRecall:%f\tF1:%f' % (p, r, f)

    auc = eval.ROC_rank(scoreList, labelList)
    print 'AUC:%f' % (auc)

    '''
    treeList = cart_main(irisTrainData)
    print "tree size:", len(treeList)

    for i, tree in enumerate(treeList):
        svg = TreeSVG(tree)
        svg.draw()
        outSvg = open('test-cart-%d.svg' % (i), 'w')
        print >> outSvg, svg.pageHtml
    '''




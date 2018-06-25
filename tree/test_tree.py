#coding:utf-8
import sys
from load_data import *
from cart import *
import random as rand
from TreeSVG import TreeSVG
import time

conf = {}
conf['rate'] = 0.9

if __name__ == '__main__':

    irisDataFile = sys.argv[1]

    irisData, idx2label, label2idx = load_iris_data(irisDataFile)

    print idx2label, label2idx

    for i in range(len(irisData)):
        irisData[i].insert(-1, i)
        irisData[i][-1] = 0 if irisData[i][-1] == 1 else 1

    rand.seed(int(time.time()))
    rand.shuffle(irisData)

    trainSize = int(len(irisData) * conf['rate'])
    irisTrainData = irisData[:trainSize]
    irisTestData = irisData[trainSize:]

    print "train size: %d test size: %d" % (len(irisTrainData), len(irisTestData))


    head = cart_main(irisTrainData)

    svg = TreeSVG(head)
    svg.draw()

    outSvg = open('test-cart.svg', 'w')
    print >> outSvg, svg.pageHtml






#coding:utf8

import sys
import math
import random

EPSILON=1.e-6

class Logistic:
    def __init__(self, m):
        self.w = []
        for i in range(m):
            self.w.append(random.uniform(0, 1))

    def sigmoid(self, x):
        if x < -700.:
            x = -700.
        # print "sigmoid:", x
        return 1. / (1. + math.exp(-x))

    def train(self, y, fea, rho):
        sum = 0.
        for w, x in zip(self.w, fea):
            sum += w * x

        # print sum
        p = self.sigmoid(sum)
        # print p

        for i in range(len(self.w)):
            g = 2 * (p - y) * p * (1 - p) * fea[i]
            self.w[i] = self.w[i] - rho * g

        return pow(y - p, 2), g
    def predict(self, fea):
        sum = 0.
        for w, x in zip(self.w, fea):
            sum += w * x
        p = self.sigmoid(sum)
        return p


    def loss(self, y, fea):
        sum = 0.
        for w, x in zip(self.__w, fea):
            sum += w * x
        p = self.sigmoid(sum)
        return pow(y - p, 2)

#coding:utf-8
import sys
import numpy as np

from tools import *

class rnn:
    def __init__(self, in_dim, hidden_dim, out_dim, eta):
        self.in_dim = in_dim
        self.hidden_dim = hidden_dim
        self.out_dim = out_dim
        self.eta = eta

        self.W = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.U = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.V = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, self.hidden_dim)), dtype=np.float64) 
        self.bh = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, 1)), dtype=np.float64)
        self.s_0 = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)

    def train(self, X, y):
        T = X.shape[1]
        P, S = self.forward(X)

        dl_dV = np.matrix(np.zeros((self.V.shape), dtype=np.float64))
        dl_dU = np.matrix(np.zeros((self.U.shape), dtype=np.float64))
        dl_dW = np.matrix(np.zeros((self.W.shape), dtype=np.float64))
        dl_dbh = np.matrix(np.zeros((self.bh.shape), dtype=np.float64))
        dl_db = np.matrix(np.zeros((self.b.shape), dtype=np.float64))

        y_v = np.matrix(np.zeros((self.out_dim, 1)))
        y_v[y,0] = 1

        dl_da = P[:,-1] - y_v
        dl_dV = dl_da * S[:,T - 1].T
        dl_db = dl_da
        dl_ds = self.V.T * dl_da

        # bptt
        for t in np.arange(T)[::-1]:
            if t < T - 1:
                dl_ds = self.U.T * dl_dh
            dl_dh = odot(dl_ds, tanh_(S[:,t]))
            dl_dU += dl_dh * S[:,t].T 
            dl_dW += dl_dh * X[:,t].T
            dl_dbh += dl_dh

        self.U += -1 * self.eta * dl_dU
        self.V += -1 * self.eta * dl_dV
        self.W += -1 * self.eta * dl_dW
        self.s_0 += -1 * self.eta * self.U.T * dl_dh
        self.bh += -1 * self.eta * dl_dbh
        self.b  += -1 * self.eta * dl_db

        # print P[:,-1], y
        return -1. * np.log(P[:,-1][y])

    def predict(self, X):
        P, S = self.forward(X)
        return np.argmax(P[:,-1])

    def forward(self, X):
        T = X.shape[1]
        H = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        S = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))
        A = np.matrix(np.zeros((self.out_dim, T), dtype=np.float64))
        P = np.matrix(np.zeros((self.out_dim, T), dtype=np.float64))
        S[:,-1] = self.s_0

        for t in range(T):
            H[:,t] = self.W * X[:,t] + self.U * S[:,t-1] + self.bh
            S[:,t] = np.tanh(H[:,t])
            A[:,t] = self.V * S[:,t] + self.b
            P[:,t] = softmax(A[:,t])
        return P, S

if __name__ == '__main__':
    pass
    

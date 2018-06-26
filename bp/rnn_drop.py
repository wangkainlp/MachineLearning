#coding:utf-8
import sys
import numpy as np

from tools import *

class rnn_drop:
    def __init__(self, in_dim, hidden_dim, out_dim, eta):
        self.in_dim = in_dim
        self.hidden_dim = hidden_dim
        self.out_dim = out_dim
        self.rate = 0.8
        self.eta = eta

        self.W = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.U = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.V = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, self.hidden_dim)), dtype=np.float64) 
        self.bh = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, 1)), dtype=np.float64)
        self.s_0 = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)

    def train(self, X, y):
        T = X.shape[1]
        P, S, D, B = self.forward(X, 1)

        dl_dV = np.matrix(np.zeros((self.V.shape), dtype=np.float64))
        dl_dU = np.matrix(np.zeros((self.U.shape), dtype=np.float64))
        dl_dW = np.matrix(np.zeros((self.W.shape), dtype=np.float64))
        dl_dbh = np.matrix(np.zeros((self.bh.shape), dtype=np.float64))
        dl_db = np.matrix(np.zeros((self.b.shape), dtype=np.float64))

        dl_dh = np.matrix(np.zeros((self.hidden_dim, 1), dtype=np.float64))

        y_v = np.matrix(np.zeros((self.out_dim, 1)))
        y_v[y,0] = 1

        '''
        dl_da = P[:,-1] - y_v
        dl_dV = dl_da * S[:,T - 1].T
        dl_db = dl_da
        dl_ds = self.V.T * dl_da
        '''
        dl_da = P[:,-1] - y_v
        dl_db = dl_da
        dl_dd = self.V.T * dl_da
        dl_dV = dl_da * D[:,T - 1].T
        dl_ds = odot(dl_dd, B[:,T-1]) * self.rate

        # bptt
        for t in np.arange(T)[::-1]:
            if t < T - 1:
                dl_ds = self.U.T * dl_dh
            #

            dl_dh = odot(dl_ds, tanh_(S[:,t]))

            '''
            for i in range(dl_dh.shape[0]):
                if dl_dh[i,0] > 1.0:
                    dl_dh[i,0] = dl_dh[i,0] * 0.5
            '''

            dl_dU += dl_dh * S[:,t].T 
            dl_dW += dl_dh * X[:,t].T
            dl_dbh += dl_dh
            # print dl_dh.T, 
            print >> sys.stderr,  t, ':', np.sum(abs(dl_dh)) / dl_dh.shape[0]
        print >> sys.stderr, '<eos>'
        print >> sys.stderr, self.U

        dl_ds0 = self.U.T * dl_dh

        # dl 截断
        '''
        for i in range(dl_dU.shape[0]):
            for j in range(dl_dU.shape[1]):
                if dl_dU[i,j] > 1.0:
                    dl_dU[i,j] = dl_dU[i,j] * 0.5
        '''


        self.U += -1 * self.eta * dl_dU
        self.W += -1 * self.eta * dl_dW
        self.bh += -1 * self.eta * dl_dbh
        self.s_0 += -1 * self.eta * dl_ds0
        self.V += -1 * self.eta * dl_dV
        self.b  += -1 * self.eta * dl_db

        for i in range(self.U.shape[0]):
            for j in range(self.U.shape[1]):
                if self.U[i,j] > 1.0:
                    self.U[i,j] = self.U[i,j] * 0.5
        # U L2
        #self.U = self.U / np.sqrt(np.sum(odot(self.U, self.U)))
        # U L1
        # self.U = self.U / np.sqrt(np.sum(abs(self.U)))

        # print P[:,-1], y
        return -1. * np.log(P[:,-1][y])

    def predict(self, X):
        P, S, D, B = self.forward(X, 0)
        return np.argmax(P[:,-1])

    def forward(self, X, phase):
        T = X.shape[1]
        H = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        S = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))
        B = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        D = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        A = np.matrix(np.zeros((self.out_dim, T), dtype=np.float64))
        P = np.matrix(np.zeros((self.out_dim, T), dtype=np.float64))
        S[:,-1] = self.s_0

        for t in range(T):
            H[:,t] = self.W * X[:,t] + self.U * S[:,t-1] + self.bh
            S[:,t] = np.tanh(H[:,t])
            if phase == 1:
                B[:,t] = np.matrix(np.random.binomial(1, self.rate, size = self.hidden_dim)).reshape(self.hidden_dim, 1)
                D[:,t] = odot(S[:,t], B[:,t]) * (1. / self.rate)  
                A[:,t] = self.V * D[:,t] + self.b
            else:
                A[:,t] = self.V * S[:,t] + self.b
            P[:,t] = softmax(A[:,t])

        return P, S, D, B

if __name__ == '__main__':
    pass
    

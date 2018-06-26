#coding:utf-8
import sys
import numpy as np

EPSLON = 1e-6

def softmax(x):
    if x.shape[1] != 1:
        print >> stderr, "shape error! x.shape=(%d,%d)" % (x.shape[0], x.shape[1])
        sys.exit(-1)
    one = np.matrix(np.ones((x.shape[0], 1), dtype=np.float64))
    return np.exp(x) / (one.T * np.exp(x) + EPSLON)

def tanh_(x):
    return np.multiply(1 - np.tanh(x), 1 + np.tanh(x))


class rnn:
    def __init__(self, in_dim, hidden_dim, out_dim, eta):
        self.in_dim = in_dim
        self.hidden_dim = hidden_dim
        self.out_dim = out_dim
        self.eta = eta

        self.W = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.U = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.V = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, self.hidden_dim)), dtype=np.float64)
        self.s_0 = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)

        self.dl_dV = np.matrix(np.zeros((self.V.shape), dtype=np.float64))
        self.dl_dU = np.matrix(np.zeros((self.U.shape), dtype=np.float64))
        self.dl_dW = np.matrix(np.zeros((self.W.shape), dtype=np.float64))

        T = 300
        self.H = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        self.S = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))
        self.O = np.matrix(np.zeros((self.out_dim, T), dtype=np.float64))
        self.P = np.matrix(np.zeros((self.out_dim, T), dtype=np.float64))

    def train(self, X, y):
        T = X.shape[1]
        P, S = self.forward(X)

        self.dl_dV *= 0.
        self.dl_dU *= 0.
        self.dl_dW *= 0.

        y_v = np.matrix(np.zeros((self.out_dim, 1)))
        y_v[y,0] = 1

        # dl_do = np.argmax(P[:,-1], axis = 1) - y_v
        # self.dl_do = self.P[:,-1] - y_v
        self.dl_do = self.P[:,T-1] - y_v
        self.dl_ds = self.V.T * self.dl_do
        self.dl_dV = self.dl_do * self.S[:,T - 1].T
        # dl_dh = np.multiply(dl_ds, tanh_(H[T - 1]))
        self.dl_dh = np.multiply(self.dl_ds, np.multiply(1 - self.S[:,T - 1], 1 + self.S[:,T - 1]))

        for t in np.arange(T)[::-1]:
            self.dl_dU += self.dl_dh * self.S[:,t].T 
            self.dl_dW += self.dl_dh * X[:,t].T
            self.dl_dh = np.multiply(self.U.T * self.dl_dh, np.multiply(1 - self.S[:,T - 1], 1 + self.S[:,T - 1]))
        
        self.U += -1 * self.eta * self.dl_dU
        self.V += -1 * self.eta * self.dl_dV
        self.W += -1 * self.eta * self.dl_dW
        self.s_0 += -1 * self.eta * self.U.T * self.dl_dh

        # print P[:,-1], y
        # return -1. * np.log(self.P[:,-1][y])
        return -1. * np.log(self.P[:,T-1][y])

    def predict(self, X):
        P, S = self.forward(X)
        # return np.argmax(P[:,-1], axis = 0)
        # return np.argmax(P[:,-1])
        return np.argmax(P[:,len(X) - 1])

    def forward(self, X):
        T = X.shape[1]
        self.H *= 0.
        self.S *= 0.
        self.O *= 0.
        self.P *= 0.
        # S[:,-1] = np.matrix(np.zeros((self.hidden_dim, 1), dtype=np.float64))
        # self.S[:,-1] = self.s_0
        self.S[:,T - 1] = self.s_0

        for t in range(T):
            x_t = X[:,t]
            self.H[:,t] = self.W * x_t + self.U * self.S[:,t - 1]
            self.S[:,t] = np.tanh(self.H[:,t])
            self.O[:,t] = self.V * self.S[:,t]
            self.P[:,t] = softmax(self.O[:,t])
        return self.P, self.S



if __name__ == '__main__':
    pass
    

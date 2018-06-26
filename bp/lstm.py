#coding:utf-8
import sys
import numpy as np
from tools import *

class lstm:
    def __init__(self, in_dim, hidden_dim, out_dim, eta):
        self.in_dim = in_dim
        self.hidden_dim = hidden_dim
        self.out_dim = out_dim
        self.eta = eta

        self.W_i = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_f = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_o = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_g = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)

        self.U_i = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_f = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_o = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_g = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)

        self.V   = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, self.hidden_dim)), dtype=np.float64)
        self.b_i = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_f = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_o = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_g = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, 1)), dtype=np.float64)

        self.h_0 = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.c_0 = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)

    def train(self, X, y):
        T = X.shape[1]

        P, H, C, F, I, O, G = self.forward(X)

        dl_dV = np.matrix(np.zeros((self.V.shape), dtype=np.float64))

        dl_dU_i = np.matrix(np.zeros((self.U_i.shape), dtype=np.float64))
        dl_dU_o = np.matrix(np.zeros((self.U_o.shape), dtype=np.float64))
        dl_dU_f = np.matrix(np.zeros((self.U_f.shape), dtype=np.float64))
        dl_dU_g = np.matrix(np.zeros((self.U_g.shape), dtype=np.float64))

        dl_dW_i = np.matrix(np.zeros((self.W_i.shape), dtype=np.float64))
        dl_dW_o = np.matrix(np.zeros((self.W_o.shape), dtype=np.float64))
        dl_dW_f = np.matrix(np.zeros((self.W_f.shape), dtype=np.float64))
        dl_dW_g = np.matrix(np.zeros((self.W_g.shape), dtype=np.float64))

        dl_db_i = np.matrix(np.zeros((self.b_i.shape), dtype=np.float64))
        dl_db_o = np.matrix(np.zeros((self.b_o.shape), dtype=np.float64)) 
        dl_db_f = np.matrix(np.zeros((self.b_f.shape), dtype=np.float64)) 
        dl_db_g = np.matrix(np.zeros((self.b_g.shape), dtype=np.float64)) 


        y_v = np.matrix(np.zeros((self.out_dim, 1)))
        y_v[y,0] = 1

        dl_da = P[:,-1] - y_v
        dl_dV = dl_da * H[:,T - 1].T
        dl_db = dl_da

        dl_dh_t = self.V.T * dl_da
        dl_dc_t = odot(odot(dl_dh_t, O[:,T-1]), tanh_(np.tanh(C[:,T-1])))
        # bptt
        for t in np.arange(T)[::-1]:
            if t < T - 1:
                dl_dc_t =  odot(dl_dc_t, F[:,t+1]) + odot3(dl_dh_t, O[:,t], tanh_(np.tanh(C[:,t])))

            dl_dzf_t = odot3(dl_dc_t, C[:,t-1], sigmoid_(F[:,t])) 
            dl_dU_f += dl_dzf_t * H[:,t-1].T
            dl_dW_f += dl_dzf_t * X[:,t].T
            dl_db_f += dl_dzf_t

            dl_dzi_t = odot3(dl_dc_t, G[:,t], sigmoid_(I[:,t]))
            dl_dU_i += dl_dzi_t * H[:,t-1].T
            dl_dW_i += dl_dzi_t * X[:,t].T
            dl_db_i += dl_dzi_t

            dl_dzg_t = odot3(dl_dc_t, I[:,t], tanh_(G[:,t]))
            dl_dU_g += dl_dzg_t * H[:,t-1].T
            dl_dW_g += dl_dzg_t * X[:,t].T
            dl_db_g += dl_dzg_t

            dl_dzo_t = odot3(dl_dh_t, np.tanh(C[:,t]), sigmoid_(O[:,t]))
            dl_dU_o += dl_dzo_t * H[:,t-1].T
            dl_dW_o += dl_dzo_t * X[:,t].T
            dl_db_o += dl_dzo_t


            dl_dh_t = self.U_f.T * dl_dzf_t + self.U_i.T * dl_dzi_t + \
                      self.U_o.T * dl_dzo_t + self.U_g.T * dl_dzg_t

        self.V += -1 * self.eta * dl_dV

        self.U_f += -1 * self.eta * dl_dU_f
        self.U_i += -1 * self.eta * dl_dU_i
        self.U_g += -1 * self.eta * dl_dU_g
        self.U_o += -1 * self.eta * dl_dU_o

        self.W_f += -1 * self.eta * dl_dW_f
        self.W_i += -1 * self.eta * dl_dW_i
        self.W_g += -1 * self.eta * dl_dW_g
        self.W_o += -1 * self.eta * dl_dW_o

        self.b_f += -1 * self.eta * dl_db_f
        self.b_i += -1 * self.eta * dl_db_i
        self.b_g += -1 * self.eta * dl_db_g
        self.b_o += -1 * self.eta * dl_db_o

        self.b += -1 * self.eta * dl_db

        self.c_0 += -1 * self.eta * odot(dl_dc_t, F[:,0])
        self.h_0 += -1 * self.eta * dl_dh_t

        # print P[:,-1], y
        return -1. * np.log(P[:,-1][y])

    def predict(self, X):
        P, H, C, F, I, O, G = self.forward(X)
        # return np.argmax(P[:,-1], axis = 0)
        return np.argmax(P[:,-1])

    def forward(self, X):
        T = X.shape[1]
        I = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        O = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        F = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        G = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
    
        C = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))
        H = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))
        A = np.matrix(np.zeros((self.out_dim, T), dtype=np.float64))
        P = np.matrix(np.zeros((self.out_dim, T), dtype=np.float64))
        C[:,-1] = self.c_0
        H[:,-1] = self.h_0

        for t in range(T):
            F[:,t] = sigmoid(self.U_f * H[:,t-1] + self.W_f * X[:,t] + self.b_f)
            I[:,t] = sigmoid(self.U_i * H[:,t-1] + self.W_i * X[:,t] + self.b_i)
            O[:,t] = sigmoid(self.U_o * H[:,t-1] + self.W_o * X[:,t] + self.b_o)
            G[:,t] = np.tanh(self.U_g * H[:,t-1] + self.W_g * X[:,t] + self.b_g)

            C[:,t] = odot(F[:,t], C[:,t-1]) + odot(I[:,t], G[:,t])
            H[:,t] = odot(O[:,t], np.tanh(C[:,t]))

            A[:,t] = self.V * H[:,t] + self.b
            P[:,t] = softmax(A[:,t])

        return P, H, C, F, I, O, G

if __name__ == '__main__':
    pass
    

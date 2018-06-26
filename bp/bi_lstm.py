#coding:utf-8
import sys
import numpy as np
from tools import *

class bi_lstm:
    def __init__(self, in_dim, hidden_dim, out_dim, eta):
        self.in_dim = in_dim
        self.hidden_dim = hidden_dim
        self.out_dim = out_dim
        self.eta = eta
        self.rate = 0.8

        self.W_i = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_f = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_o = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_g = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)

        self.U_i = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_f = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_o = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_g = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)

        self.b_i = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_f = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_o = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_g = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)

        self.h_0 = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.c_0 = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)

        self.W_i_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_f_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_o_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)
        self.W_g_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.in_dim)), dtype=np.float64)

        self.U_i_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_f_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_o_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)
        self.U_g_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, self.hidden_dim)), dtype=np.float64)

        self.b_i_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_f_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_o_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.b_g_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)

        self.h_0_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64)
        self.c_0_ = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.hidden_dim, 1)), dtype=np.float64) 


        self.b = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, 1)), dtype=np.float64)
        self.V   = np.matrix(np.random.uniform(-1.0, 1.0, size=(self.out_dim, self.hidden_dim * 2)), dtype=np.float64)

    def train(self, X, y):
        T = X.shape[1]
        X_ = X[::-1]

        P, A, D, Mask, CH, H, C, F, I, O, G, H_, C_, F_, I_, O_, G_ = self.forward(X, 1)

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

        #
        dl_dU_i_ = np.matrix(np.zeros((self.U_i_.shape), dtype=np.float64))
        dl_dU_o_ = np.matrix(np.zeros((self.U_o_.shape), dtype=np.float64))
        dl_dU_f_ = np.matrix(np.zeros((self.U_f_.shape), dtype=np.float64))
        dl_dU_g_ = np.matrix(np.zeros((self.U_g_.shape), dtype=np.float64))

        dl_dW_i_ = np.matrix(np.zeros((self.W_i_.shape), dtype=np.float64))
        dl_dW_o_ = np.matrix(np.zeros((self.W_o_.shape), dtype=np.float64))
        dl_dW_f_ = np.matrix(np.zeros((self.W_f_.shape), dtype=np.float64))
        dl_dW_g_ = np.matrix(np.zeros((self.W_g_.shape), dtype=np.float64))

        dl_db_i_ = np.matrix(np.zeros((self.b_i_.shape), dtype=np.float64))
        dl_db_o_ = np.matrix(np.zeros((self.b_o_.shape), dtype=np.float64)) 
        dl_db_f_ = np.matrix(np.zeros((self.b_f_.shape), dtype=np.float64)) 
        dl_db_g_ = np.matrix(np.zeros((self.b_g_.shape), dtype=np.float64)) 



        y_v = np.matrix(np.zeros((self.out_dim, 1)))
        y_v[y,0] = 1

        '''
        dl_da = P - y_v
        dl_db = dl_da
        dl_dV = dl_da * CH.T
        dl_dch_t = self.V.T * dl_da
        '''
        # dropout layer
        dl_da = P - y_v
        dl_db = dl_da
        dl_dd = self.V.T * dl_da
        dl_dV = dl_da * D.T
        # dl_dch_t = odot(dl_dd, B) * self.rate
        dl_dch_t = odot(dl_dd, Mask) * self.rate
        # dropout

        dl_dh_t = dl_dch_t[:self.hidden_dim,:]
        dl_dh_t_ = dl_dch_t[self.hidden_dim:,:]

        dl_dc_t = odot(odot(dl_dh_t, O[:,T-1]), tanh_(np.tanh(C[:,T-1])))
        dl_dc_t_ = odot(odot(dl_dh_t_, O_[:,T-1]), tanh_(np.tanh(C_[:,T-1])))

        # bptt
        for t in np.arange(T)[::-1]:
            if t < T - 1:
                dl_dc_t =  odot(dl_dc_t, F[:,t+1]) + odot3(dl_dh_t, O[:,t], tanh_(np.tanh(C[:,t])))
                dl_dc_t_ =  odot(dl_dc_t_, F_[:,t+1]) + odot3(dl_dh_t_, O_[:,t], tanh_(np.tanh(C_[:,t])))

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

            # backward
            dl_dzf_t_ = odot3(dl_dc_t_, C_[:,t-1], sigmoid_(F_[:,t])) 
            dl_dU_f_ += dl_dzf_t_ * H_[:,t-1].T
            dl_dW_f_ += dl_dzf_t_ * X_[:,t].T
            dl_db_f_ += dl_dzf_t_

            dl_dzi_t_ = odot3(dl_dc_t_, G_[:,t], sigmoid_(I_[:,t]))
            dl_dU_i_ += dl_dzi_t_ * H_[:,t-1].T
            dl_dW_i_ += dl_dzi_t_ * X_[:,t].T
            dl_db_i_ += dl_dzi_t_

            dl_dzg_t_ = odot3(dl_dc_t_, I_[:,t], tanh_(G_[:,t]))
            dl_dU_g_ += dl_dzg_t_ * H_[:,t-1].T
            dl_dW_g_ += dl_dzg_t_ * X_[:,t].T
            dl_db_g_ += dl_dzg_t_

            dl_dzo_t_ = odot3(dl_dh_t_, np.tanh(C_[:,t]), sigmoid_(O_[:,t]))
            dl_dU_o_ += dl_dzo_t_ * H_[:,t-1].T
            dl_dW_o_ += dl_dzo_t_ * X_[:,t].T
            dl_db_o_ += dl_dzo_t_

            dl_dh_t_ = self.U_f_.T * dl_dzf_t_ + self.U_i.T * dl_dzi_t_ + \
                      self.U_o_.T * dl_dzo_t_ + self.U_g_.T * dl_dzg_t_





        self.V += -1 * self.eta * dl_dV
        self.b += -1 * self.eta * dl_db

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

        self.c_0 += -1 * self.eta * odot(dl_dc_t, F[:,0])
        self.h_0 += -1 * self.eta * dl_dh_t
        
        self.U_f_ += -1 * self.eta * dl_dU_f_
        self.U_i_ += -1 * self.eta * dl_dU_i_
        self.U_g_ += -1 * self.eta * dl_dU_g_
        self.U_o_ += -1 * self.eta * dl_dU_o_
                                            
        self.W_f_ += -1 * self.eta * dl_dW_f_
        self.W_i_ += -1 * self.eta * dl_dW_i_
        self.W_g_ += -1 * self.eta * dl_dW_g_
        self.W_o_ += -1 * self.eta * dl_dW_o_
                                            
        self.b_f_ += -1 * self.eta * dl_db_f_
        self.b_i_ += -1 * self.eta * dl_db_i_
        self.b_g_ += -1 * self.eta * dl_db_g_
        self.b_o_ += -1 * self.eta * dl_db_o_


        self.c_0_ += -1 * self.eta * odot(dl_dc_t_, F_[:,0])
        self.h_0_ += -1 * self.eta * dl_dh_t_


        # print P[:,-1], y
        return -1. * np.log(P[y])

    def predict(self, X):
        # P, H, C, F, I, O, G = self.forward(X)
        P, A, D, Mask, CH, H, C, F, I, O, G, H_, C_, F_, I_, O_, G_ = self.forward(X, 0)
        # return np.argmax(P[:,-1], axis = 0)
        return np.argmax(P[:,-1])

    def forward(self, X, phase):
        T = X.shape[1]
        X_ = X[::-1]

        I = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        O = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        F = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        G = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
    
        C = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))
        H = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))

        C[:,-1] = self.c_0
        H[:,-1] = self.h_0

        I_ = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        O_ = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        F_ = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
        G_ = np.matrix(np.zeros((self.hidden_dim, T), dtype=np.float64))
    
        C_ = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))
        H_ = np.matrix(np.zeros((self.hidden_dim, T + 1), dtype=np.float64))

        C_[:,-1] = self.c_0_
        H_[:,-1] = self.h_0_

        # output layer
        CH = np.matrix(np.zeros((self.hidden_dim * 2, 1), dtype=np.float64))
        Mask = np.matrix(np.zeros((self.hidden_dim * 2, 1), dtype=np.float64))
        D = np.matrix(np.zeros((self.hidden_dim * 2, 1), dtype=np.float64))
        A = np.matrix(np.zeros((self.out_dim, 1), dtype=np.float64))
        P = np.matrix(np.zeros((self.out_dim, 1), dtype=np.float64))

        for t in range(T):
            # forward
            F[:,t] = sigmoid(self.U_f * H[:,t-1] + self.W_f * X[:,t] + self.b_f)
            I[:,t] = sigmoid(self.U_i * H[:,t-1] + self.W_i * X[:,t] + self.b_i)
            O[:,t] = sigmoid(self.U_o * H[:,t-1] + self.W_o * X[:,t] + self.b_o)
            G[:,t] = np.tanh(self.U_g * H[:,t-1] + self.W_g * X[:,t] + self.b_g)

            C[:,t] = odot(F[:,t], C[:,t-1]) + odot(I[:,t], G[:,t])
            H[:,t] = odot(O[:,t], np.tanh(C[:,t]))

            # backward
            F_[:,t] = sigmoid(self.U_f_ * H_[:,t-1] + self.W_f_ * X_[:,t] + self.b_f_)
            I_[:,t] = sigmoid(self.U_i_ * H_[:,t-1] + self.W_i_ * X_[:,t] + self.b_i_)
            O_[:,t] = sigmoid(self.U_o_ * H_[:,t-1] + self.W_o_ * X_[:,t] + self.b_o_)
            G_[:,t] = np.tanh(self.U_g_ * H_[:,t-1] + self.W_g_ * X_[:,t] + self.b_g_)

            C_[:,t] = odot(F_[:,t], C_[:,t-1]) + odot(I_[:,t], G_[:,t])
            H_[:,t] = odot(O_[:,t], np.tanh(C_[:,t]))

        CH[:self.hidden_dim,:] = H[:,-1]
        CH[self.hidden_dim:,:] = H_[:,-1]

        if phase == 1:
            Mask[:,:] = np.matrix(np.random.binomial(1, self.rate, size = self.hidden_dim * 2)).reshape(self.hidden_dim * 2, 1)
            D[:,:] = odot(CH, Mask) * (1. / self.rate)  
            A = self.V * D + self.b
        else: 
            A = self.V * CH + self.b
        P = softmax(A)

        # return P, A, CH, H, C, F, I, O, G_, H_, C_, F_, I_, O_, G_
        return P, A, D, Mask, CH, H, C, F, I, O, G_, H_, C_, F_, I_, O_, G_

if __name__ == '__main__':
    pass
    

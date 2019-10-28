#coding:utf-8
import numpy as np
import time


def isclose(a, b, tol = 1e-7):
    return abs(a - b) < tol

# 0 -- 线性核函数: K(u,v)=u'*v
# 1 -- 多项式核函数: K(u,v)=(gamma*u'*v + coef0)^d
# 2 -- RBF核函数: K(u,v)=exp(-gamma*||u-v||^2)
# 3 -- sigmoid核函数: K(u,v)=tanh(gamma*u'*v + coef0)
def kernel(x1, x2, s = 2):
    if s == 0:
        return np.dot(x1, x2)
    elif s == 1:
        gamma = 1.0 / 2     # 默认值是1/k（k是类别数）
        c = 1               # 默认值是0
        d = 3               # 默认3
        return (gamma * np.dot(x1, x2) + c) ** d
    elif s == 2:
        gamma = 1.0 / 2     # 默认值是1/k（k是类别数）
        gamma = 10
        return np.exp(-gamma * np.sqrt(np.sum( (x1 - x2) ** 2)))
        # return np.exp(-gamma * np.sum( (x1 - x2) ** 2))
    elif s == 3:
        gamma = 1.0 / 2     # 默认值是1/k（k是类别数）
        c = 0
        return np.tanh(gamma * np.dot(x1, x2) + c)


class SVM():
    def __init__(self, feature_size, C, _X, _Y):
        # self.weight = np.random.uniform(-1, 1, size=(feature_size,)).astype(np.float32)
        # self.alpha  = np.random.uniform(-1, 1, size=(feature_size,)).astype(np.float32)
        self.C = C
        self.X = _X
        self.Y = _Y
        self.b = 0.0
        self.size = _X.shape[0]
        self.epsilon = 1e-7
        self.tol = 0.001

        self.alpha  = np.zeros(shape=(self.size,)).astype(np.float32)
        print 'alpha.shape:', self.alpha.shape


    def g_x(self, idx):
        '''
        s = 0.0
        for j in range(self.size):
            s += self.alpha[j] * self.Y[j] * np.dot(self.X[j], self.X[idx])
        s += self.b
        '''
        # gs = self.alpha * self.Y * np.dot(self.X, self.X[idx])
        gs = self.alpha * self.Y * kernel(self.X, self.X[idx])
        g = gs.sum() + self.b
        return g

    def satisfyKTT(self, idx):
        '''
        alpha = 0     <==> y_i * g(x_i) >= 1
        0 < alpha < C <==> y_i * g(x_i) =  1
        alpha = C     <==> y_i * g(x_i) <= 1

        y_i * g(x_i) > 1 && alpha > 0
        y_i * g(x_i) < 1 && alpha < C
        '''

        y_i = self.Y[idx]
        g_i = self.g_x(idx)
        # print idx, 'g_i:%.3f' % (g_i), 'y_i:', y_i

        e = y_i * g_i
        # if (e > 1.0  and self.alpha[idx] > 0) or (e < 1.0 and self.alpha[idx] < self.C):
        if (e > 1.0 + self.tol  and self.alpha[idx] > 0) or (e < 1.0 - self.tol and self.alpha[idx] < self.C):
            return False
        else:
            return True

    def calE(self, idx):
        '''
        g = 0.0
        for i in range(self.size):
            g += self.alpha[i] * self.Y[i] * np.dot(self.X[i], self.X[idx])
        g += self.b
        '''
        # gs = self.alpha * self.Y * np.dot(self.X, self.X[idx])
        gs = self.alpha * self.Y * kernel(self.X, self.X[idx])
        g = gs.sum() + self.b
        return g - self.Y[idx]



    def select_j(self, idx):
        # 选择|E1 - E2|最大的alpha

        # 间隔边界
        eMax = float('-inf')
        alpha2 = -1
        for i in range(self.size):
            # if self.alpha[i] == 0.0 or self.alpha[i] == self.C:
            if isclose(self.alpha[i], 0.0) or isclose(self.alpha[i], self.C):
                pass
            else:
                E1 = self.calE(idx)
                E2 = self.calE(i)
                if abs(E1 - E2) > eMax:
                    eMax = abs(E1 - E2)
                    alpha2 = i

        if alpha2 != -1:
            return alpha2

        # 非间隔边界
        eMax = float('-inf')
        alpha2 = -1
        l = range(self.size)
        import random
        random.shuffle(l)
        for i in l:
        # for i in range(self.size):
            # if self.alpha[i] == 0.0 or self.alpha[i] == self.C:
            if isclose(self.alpha[i], 0.0) or isclose(self.alpha[i], self.C):
                E1 = self.calE(idx)
                E2 = self.calE(i)
                # print 'a2:', i, abs(E1 -E2)
                if abs(E1 - E2) > eMax:
                    eMax = abs(E1 - E2)
                    alpha2 = i
        if alpha2 != -1:
            # print 'alpha2:', alpha2, eMax
            return alpha2
        else:
            return -1

    def update(self, i, j):
        E1 = self.calE(i)
        E2 = self.calE(j)

        # print 'E1,E2:', E1, E2

        if self.Y[i] == self.Y[j]:
            L = max(0, self.alpha[j] + self.alpha[i] - self.C)
            H = min(self.C, self.alpha[j] + self.alpha[i])
        else:
            L = max(0, self.alpha[j] - self.alpha[i])
            H = min(self.C, self.C + self.alpha[j] - self.alpha[i])

        # eta = np.dot(self.X[i], self.X[i]) + np.dot(self.X[j], self.X[j]) \
        #        - 2 * np.dot(self.X[i], self.X[j])
        eta = kernel(self.X[i], self.X[i]) + kernel(self.X[j], self.X[j]) \
               - 2 * kernel(self.X[i], self.X[j])

        # if eta <= 0.0:
        if isclose(eta, 0.0):
            print 'eta:', eta
            return 0

        new_alpha2 = self.alpha[j] + self.Y[j] * (E1 - E2) / eta
        new_alpha2 = min(new_alpha2, H)
        new_alpha2 = max(new_alpha2, L)

        if abs(new_alpha2 - self.alpha[j]) < self.epsilon:
            '''
            print 'new_alpha2, old:', new_alpha2, self.alpha[j]
            print 'y1, y2:', self.Y[i], self.Y[j]
            print 'E1, E2:', E1, E2
            print 'eta:', eta
            print 'new2:', self.alpha[j] + self.Y[j] * (E1 - E2) / eta
            '''
            return 0

        new_alpha1 = self.alpha[i] + self.Y[i] * self.Y[j] * (self.alpha[j] - new_alpha2)

        if new_alpha1 < 0 and new_alpha1 > self.epsilon:
            new_alpha1 = 0.0

        # b
        # b1 = -E1 - self.Y[i] * np.dot(self.X[i], self.X[i]) * (new_alpha1 - self.alpha[i])  \
        #          - self.Y[j] * np.dot(self.X[j], self.X[i]) * (new_alpha2 - self.alpha[j])  \
        #          + self.b
        b1 = -E1 - self.Y[i] * kernel(self.X[i], self.X[i]) * (new_alpha1 - self.alpha[i])  \
                 - self.Y[j] * kernel(self.X[j], self.X[i]) * (new_alpha2 - self.alpha[j])  \
                 + self.b

        if 0 < new_alpha1 and new_alpha1 < self.C:
            self.b = b1
        else:
            if 0 < new_alpha2 and new_alpha2 < self.C:
                pass
            else:
                # b2 = -E2 - self.Y[i] * np.dot(self.X[i], self.X[j]) * (new_alpha1 - self.alpha[i])  \
                #          - self.Y[j] * np.dot(self.X[j], self.X[j]) * (new_alpha2 - self.alpha[j])  \
                #          + self.b
                b2 = -E2 - self.Y[i] * kernel(self.X[i], self.X[j]) * (new_alpha1 - self.alpha[i])  \
                         - self.Y[j] * kernel(self.X[j], self.X[j]) * (new_alpha2 - self.alpha[j])  \
                         + self.b
                self.b = (b1 + b2) / 2

        # print 'alpha2:', self.alpha[j], new_alpha2
        # print 'alpha1:', self.alpha[i], new_alpha1
        self.alpha[i] = new_alpha1
        self.alpha[j] = new_alpha2

        return 1

    def train(self, M = 100):

        epcho = 0
        for epcho in range(M):
            print '-' * 50
            print 'epoch:', epcho
            epcho += 1
            change_cnt = 0
            boundarySat = True
            for i in range(self.size):
                # if self.alpha[i] == 0.0 or self.alpha[i] == self.C:
                if isclose(self.alpha[i], 0.0) or isclose(self.alpha[i], self.C):
                    pass
                else:
                    # print 'boundary:', i
                    # 间隔边界上
                    if not self.satisfyKTT(i):
                        # print 'not sat KTT:', i
                        boundarySat = False
                        j = self.select_j(i)
                        # print 'j:', j
                        if j == -1:
                            # print 'j:', j
                            pass
                        else:
                            change_cnt += self.update(i, j)
                    else:
                        pass
                        # print 'sat KTT:', i

            if boundarySat != True:
                continue

            # if change_cnt <= 0:
            #     break

            notBoudarySat = True
            for i in range(self.size):
                # if self.alpha[i] == 0.0 or self.alpha[i] == self.C:
                if isclose(self.alpha[i], 0.0) or isclose(self.alpha[i], self.C):
                    # print 'no boundary:', i
                    # 非间隔边界上
                    if not self.satisfyKTT(i):
                        # print 'not sat KTT:', i
                        notBoudarySat = False
                        j = self.select_j(i)
                        # print 'i,j:', i, j
                        if j == -1:
                            # print 'j:', j
                            pass
                        else:
                            change_cnt += self.update(i, j)
                    else:
                        pass
                        # print 'sat KTT:', i
                else:
                    pass

            if boundarySat == True and notBoudarySat == True:
                print 'all sat'
                break

            if change_cnt <= 0:
                print 'no change'
                break

            # print self.alpha
 
    def predict(self, x):
        g = 0.0
        for i in range(self.size):
            # g += self.alpha[i] * self.Y[i] * np.dot(self.X[i], x)
            g += self.alpha[i] * self.Y[i] * kernel(self.X[i], x)
        g += self.b

        return 1 if g >= 0 else -1, g
    
    def getWeight(self):
        w = np.zeros((self.X.shape[1],)).astype(np.float32)
        for i in range(self.size):
            w += self.alpha[i] * self.Y[i] * self.X[i]

        return w, self.b






if __name__ == '__main__':
    model = SVM(10, 3)


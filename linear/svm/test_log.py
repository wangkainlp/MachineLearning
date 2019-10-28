#coding:utf-8
import sys
import time
import math
import numpy as np

from svm_model import SVM

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D



'''
_X = [[1, 0, 1], [0, 1, 1], [1, 1, 0]]
_y = [0, 1, 1]
'''

# _X = [[.1, 0], [0.2, 0.1], [0.3, .2], [.0, .1], [.1, .2], [.2, .3] ]
_X = [[1, 0], [2, 1], [3, 2], [0, 1], [1, 2], [2, 3] ]
# _y = [0, 0, 0, 1, 1, 1]
# _y = [-1, -1, -1, 1, 1, 1]
_y = [-1, -1, 1, 1, 1, 1]

X = np.array(_X)
Y = np.zeros((X.shape[0], len(set(_y))))
for i in range(Y.shape[0]):
    # Y[i][y[i]] = 1
    Y[i][_y[i]] = 1
Y = np.array(_y).astype(np.int32)


X1 = [ i * 0.1 - 1.0 for i in range(20 + 1) ]
X2 = [ math.sqrt(1.0 - i * i) for i in X1 ]
Y  = [ -1 for i in X1 ]

X1_ = [ i * 0.2 - 2.0 for i in range(20 + 1) ]
X2_ = [ math.sqrt(4.0 - i * i) for i in X1_ ]
Y_  = [ 1 for i in X1_ ]

X1.extend(X1_)
X2.extend(X2_)
Y.extend(Y_)
X = [ (x1, x2) for x1, x2 in zip(X1, X2) ]
X = np.array(X)
Y = np.array(Y)

_w = 0.963255
_b = 1.89471795539

'''
sub_x = [ 0.1 * it - 3.0 for it in range(60) ]
sub_y = [ _w * it + _b for it in sub_x ]

plt.plot(sub_x, sub_y, color='red', label='testing accuracy')

plt.scatter( [ X[i][0] for i in range(X.shape[0]) if Y[i] == -1] , [ X[i][1] for i in range(X.shape[0]) if Y[i] == -1], s=20, color='blue', marker='o')

plt.scatter( [ X[i][0] for i in range(X.shape[0]) if Y[i] == 1] , [ X[i][1] for i in range(X.shape[0]) if Y[i] == 1], s=20, color='red', marker='*')

plt.show()
sys.exit(1)
'''


'''
plt.scatter( [ X[i][0] for i in range(X.shape[0]) if Y[i] == -1] , [ X[i][1] for i in range(X.shape[0]) if Y[i] == -1], s=20, color='blue', marker='o')

plt.scatter( [ X[i][0] for i in range(X.shape[0]) if Y[i] == 1] , [ X[i][1] for i in range(X.shape[0]) if Y[i] == 1], s=20, color='red', marker='*')

plt.show()
sys.exit(1)
'''


'''
import load_iris 
_X, Y = load_iris.load_data('iris.csv')

X = np.zeros((_X.shape[0], 2)).astype(np.float32)
for i in range(X.shape[0]):
    X[i,:] = _X[i, 0 : 2]

for i in range(Y.shape[0]):
    Y[i] = 1 if Y[i] == 0 else -1 
'''


# logistic
'''
Y = np.zeros((X.shape[0], 1)).astype(np.float32)
for i in range(Y.shape[0]):
    Y[i] = _y[i]
'''

model = SVM(X.shape[1], 1.0, X, Y)
# model = LogsticBi(X.shape[1], 0.3)

print X.shape
print Y.shape

'''
pred, prob = model.predict(X)
print pred
print prob

print model.loss(Y, prob)
'''

XX = np.copy(X)
YY = np.copy(Y)

cost = 10
epoch = 0

# model.fit(X, Y, 20)
# model.fit_mini_batch(X, Y, 10, 4)
# model.fit_multi_process(X, Y, 300, 4)
# model.fit_by_svrg(X, Y, 10, 3)
model.train(M=50)

print 'model.alpha:', model.alpha
print 'model.b:', model.b


cnt = 0
for i in range(XX.shape[0]):
    p, s = model.predict(XX[i])
    print YY[i], p, s
    if p == YY[i]:
        cnt += 1
    Y[i] = p
print 'precise:', 1.0 * cnt / XX.shape[0]

w, b = model.getWeight()
'''
_w = -w[0] / w[1]
_b = -b    / w[1]
print _w, _b
'''

print w, b

'''
_w0 = -w[0] / w[2]
_w1 = -w[1] / w[2]
_b    = -b / w[2]

sub_x = np.linspace(1, 10, 10)
sub_y = np.linspace(1, 10, 10)
sub_x, sub_y = np.meshgrid(sub_x, sub_y)

sub_z = _w0 * sub_x + _w1 * sub_y + _b
'''


# sub_x = [ 0.1 * it for it in range(100) ]
sub_x = [ 0.1 * it - 3.0 for it in range(60) ]
sub_y = [ _w * it + _b for it in sub_x ]

# plt.plot(sub_x, sub_y, color='red', label='testing accuracy')

plt.scatter( [ X[i][0] for i in range(X.shape[0]) if Y[i] == -1] , [ X[i][1] for i in range(X.shape[0]) if Y[i] == -1], s=20, color='blue', marker='o')

plt.scatter( [ X[i][0] for i in range(X.shape[0]) if Y[i] == 1] , [ X[i][1] for i in range(X.shape[0]) if Y[i] == 1], s=20, color='red', marker='*')

'''

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter([ X[i][0] for i in range(X.shape[0]) if Y[i] == -1] , \
        [ X[i][1] for i in range(X.shape[0]) if Y[i] == -1], \
        [ X[i][2] for i in range(X.shape[0]) if Y[i] == -1], \
        s=20, color='blue', marker='o')

ax.scatter([ X[i][0] for i in range(X.shape[0]) if Y[i] == 1] , \
        [ X[i][1] for i in range(X.shape[0]) if Y[i] == 1], \
        [ X[i][2] for i in range(X.shape[0]) if Y[i] == 1], \
        s=20, color='red', marker='*')

# ax.plot(sub_x, sub_y, color='red', label='testing accuracy')
ax.plot_surface(sub_x, sub_y, sub_z, rstride=1, cstride=1, linewidth=0, shade=False, alpha=0.5)
'''

'''
import math
sub_x = [ 0.1 * it - 10  for it in range(0, 200) ]
sub_y = [ 0.1 * it - 10  for it in range(0, 200) ]
sub_z = [ math.sqrt(x * x + y * y)  for x, y in zip(sub_x, sub_y) ]


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot(sub_x, sub_y, sub_z)
# ax.scatter(sub_x, sub_y, sub_z, rstride=1, cstride=1)
'''


plt.show()




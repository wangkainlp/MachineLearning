import numpy as np

'''
x = np.matrix(np.ones((300,300), dtype=np.float64))
y = np.matrix(np.ones((300,300), dtype=np.float64))
for i in range(10000):
    z = x * x
'''

x = np.ones((300,300), dtype=np.float64)
y = np.ones((300,300), dtype=np.float64)
for i in range(10000):
    z = x.dot(x)

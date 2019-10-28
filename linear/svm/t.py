#coding:utf-8

import numpy as np 
import random
import matplotlib.pyplot as plt

x_axix = range(100)
test_acys = [ random.random() * 100 for i in range(100) ]

'''
# sub_axix = filter(lambda x:x%20 == 0, x_axix)
sub_axix = x_axix
plt.plot(sub_axix, test_acys, color='red', label='testing accuracy')
'''

plt.scatter(x_axix, test_acys, s=20, color='b')


# plt.legend() # 显示图例

plt.show()



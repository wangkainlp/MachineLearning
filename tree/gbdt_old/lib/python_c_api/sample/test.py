#coding:utf-8

'''
import spam
# status = spam.system("ls -l")

# print status
'''

import tree

import time

# data = range(1000000)
data = range(20, -1, -1)

data = [ 1, 2, 3, 1, \
         2, 3, 5, 0, \
         5, 8, 1, 0, \
         2, 1, 3, 1, \
         3, 5, 2, 1 ]

data = [0, 0, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 1, 0, 1, 1,
        0, 1, 1, 0, 1,
        0, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 1, 0,
        1, 1, 1, 1, 1,
        1, 0, 1, 2, 1,
        1, 0, 1, 2, 1,
        2, 0, 1, 2, 1,
        2, 0, 1, 1, 1,
        2, 1, 0, 1, 1,
        2, 1, 0, 2, 1,
        2, 0, 0, 0, 0 ]

begin = time.time()
# tree.testdata([0, 1, 2, 3, 4], 2, 2)
# tree.testdata(data, 10000, 100)

for i in range(15):
    for j in range(5):
        print "%d\t" % (data[ i * 5 + j]),
    print ''


tree.testdata(data, 15, 5, range(15), range(4))

print "run ", time.time() - begin

'''
import noddy

n = noddy.Noddy()
n.first = "a"

print n.test()
'''



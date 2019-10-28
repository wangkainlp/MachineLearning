#coding:utf-8

'''
import spam
# status = spam.system("ls -l")

# print status
'''

from tree_node import TreeNode
from TreeSVG import * 
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


def copy_tree(tree_list, idx, root):
    copy(tree_list, idx, root)

    if (2 * idx + 1) < len(tree_list):
        if len(tree_list[2 * idx + 1]) > 0:
            left = TreeNode()
            root.left = left
            copy_tree(tree_list, 2 * idx + 1, left)

    if 2 * idx + 2 < len(tree_list):
        if len(tree_list[2 * idx + 2]) > 0:
            right = TreeNode()
            root.right = right
            copy_tree(tree_list, 2 * idx + 2, right)


def copy(tree_list, idx, tmp):
    if len(tree_list[idx]) > 0:
        tmp.split = tree_list[idx]['split']
        tmp.idx   = tree_list[idx]['fIdx']
        tmp.value = tree_list[idx]['value']
        tmp.depth = tree_list[idx]['depth']
        tmp.size  = tree_list[idx]['size']



tree_list = tree.testdata(data, 15, 5)

root = TreeNode()
copy_tree(tree_list, 0, root)

pain = TreeSVG(root)
pain.draw()

outSvg = open('test.svg', 'w') 
print >> outSvg, pain.pageHtml


print "run ", time.time() - begin

'''
import noddy

n = noddy.Noddy()
n.first = "a"

print n.test()
'''

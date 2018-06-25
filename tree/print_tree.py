#coding : utf-8
import sys
from tree_node import TreeNode

def getDepth(root):

    depth = 0
    s = []
    s.append(s)
    while len(s) > 0:
        depth += 1
        newS = []
        for it in s:
            if it.left != None:
                newS.append(it.left)
            if it.right != None:
                newS.append(it.right)
        s = newS
    return depth


def printTree(root):
    depth = getDepth(root)
    width = 2 ** (depth - 1)


if __name__ == '__main__':






#coding:utf-8

import sys

def getLeaves(root):
    leaves = [] 
    nodes = [root]
    while len(nodes) > 0:
        newNodes = []
        for it in nodes:
            if it.left != None:
                newNodes.append(it.left)
            if it.right != None:
                newNodes.append(it.right)
            if it.left == None and it.rihgt == None:
                leaves.append(it)
        nodes = newNodes
    return leaves

def getCost(root):
    leaves = [] 
    nodes = [root]
    while len(nodes) > 0:
        newNodes = []
        for it in nodes:
            if it.left != None:
                newNodes.append(it.left)
            if it.right != None:
                newNodes.append(it.right)
        nodes = newNodes
    return leaves





if __name__ == '__main__':



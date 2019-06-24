#coding:utf-8

import sys
from tree_node import *

def getLeaves(root):
    leaves = [] 
    nodes = [root]
    while len(nodes) > 0:
        newNodes = []
        for it in nodes:
            if not it.left and not it.right:
                leaves.append(it)
            if it.left:
                newNodes.append(it.left)
            if it.right:
                newNodes.append(it.right)
        nodes = newNodes
    return leaves

def getCost(data, root):
    levelList = []
    levelList.append(root)
    print len(data)

    g_t = []
    # bottomUpTraverse
    bottom2TopTraverse(data, levelList, g_t)

    return sorted(g_t, key=lambda g : g[0])

def bottom2TopTraverse(data, levelList, g_t):
    if not levelList:
        return None
    newLevelList = []
    for node in levelList:
        if node.left:
            newLevelList.append(node.left)
        if node.right:
            newLevelList.append(node.right)

    bottom2TopTraverse(data, newLevelList, g_t)

    # visit
    for node in levelList:
        # print "%d, %f" % (node.idx, node.split)
        if node.left and node.right:
            predSubtreeCost(data, node, g_t)

'''
def getSubtreeCost(subtree):

    if subtree == None:
        return 0.0

    if subtree.left == None and subtree.right == None:
        sampleScoreList = [ data[id][-1] for id in subtree.itemIds ]
        pred = 1.0 * sum(sampleScoreList) / len(sampleScoreList)
        cost = sum([ (score - pred) ** 2 for score in sampleScoreList ])
        return cost
        
    return getSubtreeCost(subtree.left) + getSubtreeCost(subtree.right)
'''

def predSubtreeCost(data, subtree, g_t):

    leaves = getLeaves(subtree)
    T_leaves = len(leaves)

    items = []
    for leaf in leaves:
        for id in leaf.itemIds:
            items.append(data[id][-1])
    pred = sum(items) / len(items)

    C_t = 0.0
    for it in items:
        C_t += (it - pred) ** 2

    C_T = 0.0
    for leaf in leaves:
        scoreList = [ data[id][-1] for id in leaf.itemIds ]
        pred = 1.0 * sum(scoreList) / len(scoreList)
        C_T += sum([(score - pred) ** 2  for score in scoreList])

    gt = 1.0 * (C_t - C_T) / (T_leaves - 1)
    g_t.append((gt, subtree.id))

def prune(data, tree, nodeId):
    levelList = [tree]
    flag = True
    while len(levelList) > 0 and flag == True:
        newLevelList = []
        for node in levelList:
            if node.id == nodeId:
                leaves = getLeaves(node)
                ids = []
                for leaf in leaves:
                    ids.extend(leaf.itemIds)

                valueSum = 0.0
                for i in ids:
                    valueSum += data[i][-1]
                node.itemIds = list(ids)
                node.value = valueSum / len(ids)
                node.left = None
                node.right = None
                node.idx = -1
                node.split = float('inf')
                flag = False
            if node.left:
                newLevelList.append(node.left)
            if node.right:
                newLevelList.append(node.right)
        levelList = newLevelList



                


def ccp(data, root):
    k = 0
    g_t = getCost(data, root)
    print g_t
    print sorted(g_t, key = lambda x : x[1])
    min_alpha = g_t[0][0]

    g_t_alpha = sorted(list(set([ x[0] for x in g_t ])))
    print g_t_alpha

    treeList = [root]
    tree = root
    for alpha in g_t_alpha:
        newTree = cloneTree(tree)
        for a, i in g_t:
            if alpha == a:
                print "min id:", i
                prune(data, newTree, i)
        tree = newTree
        treeList.append(newTree)
        if isLeaf(newTree):
            print "reach root"
            return treeList
    else:
        print "error! can not reach root"
    return treeList
                

    '''
    for alpha, i in g_t:
        if min_alpha == alpha:
            prune(data, root, i)
            print "min id:", i
            break
    '''
  



if __name__ == '__main__':
    pass



#-*- coding:utf-8 -*-
import sys
from tree_node import TreeNode 

gTreeDepth = 0

def countTreeDepth(root):
    ''' 计算树的深度
    递归计算树的深度
    Args:
        root: 根节点
    Return:
        树的深度
    '''
    if root is None:
        return 0
    leftSubTreeDepth = countTreeDepth(root.left)
    rightSubTreeDepth = countTreeDepth(root.right)
    m = leftSubTreeDepth
    if (leftSubTreeDepth < rightSubTreeDepth):
        m = rightSubTreeDepth
    return 1 + m
    '''
    m = max(leftSubTreeDepth, rightSubTreeDepth)
    return 1 + m
    '''
    

def treeDepth(root):
    '''
    计算树的深度
    Args:
        root: 树的根节点
    Returns:
        树的深度
    ''' 
    return countTreeDepth(root)

    

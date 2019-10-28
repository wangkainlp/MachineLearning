#coding:utf8

class TreeNode(object):
    def __init__(self, father = None, left = None, right = None):
        self.father = father
        self.left = left
        self.right = right
        self.split = float('inf')
        self.id = -1
        self.idx = -1
        self.value = float('inf')
        self.itemIds = None
        self.size = 0
        self.nodeSize = 0
        self.sampleSize = 0
        self.depth = 0

    def setLeftChild(self, left):
        self.left = left

    def setRightChild(self, right):
        self.right = right

    def setFather(self, father):
        self.father = father

def cloneTreeNode(newNode, node):
    newNode.__dict__ = node.__dict__.copy()

def cloneTreeLoop(newTree, root):
    if not root:
        return

    cloneTreeNode(newTree, root)
    newTree.left = None
    newTree.right = None

    if root.left:
        leftTree = TreeNode()
        cloneTreeLoop(leftTree, root.left)
        newTree.left = leftTree
        leftTree.father = newTree

    if root.right:
        rightTree = TreeNode()
        cloneTreeLoop(rightTree, root.right)
        newTree.right = rightTree
        rightTree.father = newTree

def cloneTree(root):
    newTree = TreeNode()
    cloneTreeLoop(newTree, root)
    newTree.father = None

    return newTree

def isLeaf(subtree):
    if subtree.left and subtree.right:
        return False
    return True

if __name__ == '__main__':
    n1 = TreeNode(None, None, None)
    n2 = TreeNode(n1, None, None)
    n1.setLeftChild(n2)

    print n1.split
    print n1.left.split
    print n2.father.split

#coding:utf8

class TreeNode(object):
    def __init__(self, father = None, left = None, right = None):
        self.father = father
        self.left = left
        self.right = right
        self.split = float('inf')
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


if __name__ == '__main__':
    n1 = TreeNode(None, None, None)
    n2 = TreeNode(n1, None, None)
    n1.setLeftChild(n2)

    print n1.split
    print n1.left.split
    print n2.father.split

#include <Python.h>
#include <stdlib.h>
#include <vector>
using namespace std;

#ifndef _tree_node_h_
#define _tree_node_h_

class TreeNode{
public:
    /*
    father = father
    left = left
    right = right
    split = float('inf')
    id = -1
    idx = -1
    value = float('inf')
    itemIds = None
    size = 0
    nodeSize = 0
    sampleSize = 0
    depth = 0
    */

    TreeNode() {
        father = NULL;
        left = NULL;
        right = NULL;
        value = 0.0;
        split = 0.0;
        fidx = -1;
        size = 0;
        depth = 0;
        itemIds = NULL;
    }
    ~TreeNode() {
        if (itemIds != NULL) {
            free(itemIds);
            itemIds = NULL;
        }
    }


    TreeNode* left;
    TreeNode* right;
    TreeNode* father;
    float value;
    float split;
    int fidx;
    int depth;
    int size;
    int* itemIds;
    int id;
    
};

float predict(TreeNode* root, float* feas) {
    TreeNode* curNode = root;
    while (curNode->left != NULL && curNode->right != NULL) {
        if (feas[root->fidx] < root->split) {
            curNode = curNode->left;
        } else {
            curNode = curNode->right;
        }
    }
    if (curNode->left == NULL && curNode->right == NULL) {
        return curNode->value;
    } else {
        fprintf(stderr, "node error! \nid:%d fidx:%d\n", curNode->id, curNode->fidx);
        return 0.0f;
    }
}

float boostPredict(vector<TreeNode*>& trees, float* feas, float eta) { 
    float pred = 0.0f;
    for(size_t i = 0; i < trees.size(); ++i) {
        // pred += predict(trees[i]) * eta;
        pred += predict(trees[i], feas);
    }
    return pred;
}

static PyObject* newTreeNode(TreeNode* node) {
    PyObject* pDict = PyDict_New();
    if (node == NULL) {
        return pDict;
    }
    PyDict_SetItem(pDict, Py_BuildValue("s", "split"), Py_BuildValue("f", node->split));
    PyDict_SetItem(pDict, Py_BuildValue("s", "value"), Py_BuildValue("f", node->value));
    PyDict_SetItem(pDict, Py_BuildValue("s", "fIdx"),  Py_BuildValue("i", node->fidx));
    PyDict_SetItem(pDict, Py_BuildValue("s", "depth"), Py_BuildValue("i", node->depth));
    PyDict_SetItem(pDict, Py_BuildValue("s", "size"),  Py_BuildValue("i", node->size));
    // return Py_BuildValue("O", pDict);
    return pDict;
}

static void tree2array(TreeNode* root, TreeNode** tree, int tree_size) {

    // TreeNode* tree[tree_size];

    std::vector<TreeNode*> level;
    level.push_back(root);
    std::vector<int> ids;
    ids.push_back(0);
    while (level.size()) {
        std::vector<TreeNode*> newlevel;
        std::vector<int> newids;
        for (int i = 0; i < level.size(); ++i) {
            // printf("%.2f ", level[i]->split);
            tree[ids[i]] = level[i];
            if (level[i]->left != NULL) {
                newlevel.push_back(level[i]->left);
                newids.push_back(2 * ids[i] + 1);
            }
            if (level[i]->right != NULL) {
                newlevel.push_back(level[i]->right);
                newids.push_back(2 * ids[i] + 2);
            }
        }
        // printf("\n");
        level.assign(newlevel.begin(), newlevel.end());
        ids.assign(newids.begin(), newids.end());
    }

    /*
    std::vector<int> level_int;
    level_int.push_back(0);
     while (level_int.size() > 0) {
        std::vector<int> newlevel_int;
        for (int i = 0; i < level_int.size(); ++i) {
            printf("%.2f ", tree[level_int[i]]->split);

            if (2 * level_int[i] + 1 > tree_size - 1) {
                continue;
            }

            if (tree[ 2 * level_int[i] + 1]!= NULL) {
                newlevel_int.push_back( 2 * level_int[i] + 1);
            }
            if (tree[ 2 * level_int[i] + 2]!= NULL) {
                newlevel_int.push_back( 2 * level_int[i] + 2);
            }
        }
        printf("\n");
        level_int.assign(newlevel_int.begin(), newlevel_int.end());
    }
    */

}


static void printTree(TreeNode* root) {

    if (root == NULL) {
        return;
    }

    printf("-------------------------------------------------------------------------------------------\n");
    printf("show tree:\n");
    std::vector<TreeNode * > level;
    level.push_back(root);
    int cnt = 0;
    while (level.size() > 0) {
        printf("%d: ", cnt);
        std::vector<TreeNode * > newlevel;
        for (size_t i = 0; i < level.size(); ++i) {
            if (level[i]->left != NULL) {
                newlevel.push_back(level[i]->left);
            }
            if (level[i]->right!= NULL) {
                newlevel.push_back(level[i]->right);
            }
            printf("%d:%.2f ", level[i]->fidx, level[i]->split);
        }

        bool flag = false;
        for (size_t i = 0; i < level.size(); ++i) {
            // if (level[i]->left == NULL && level[i]->right == NULL) {
                if (flag == false) {
                    flag = true;
                    printf("\n");
                }
                printf("%.3f ", level[i]->value);
            // }

        }
        printf("\n");
        level.clear();
        for (size_t i = 0; i < newlevel.size(); ++i) {
            level.push_back(newlevel[i]);
        }
        cnt += 1;
    }
}




#endif

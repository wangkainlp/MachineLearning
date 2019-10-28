#include <Python.h>
#include <stdlib.h>
#include <vector>

class Matrix {
public:
    Matrix(PyObject * data_raw, int _row, int _col) {
        row = _row;
        col = _col;
        span = _col;

        size = row * col;
        int size = PyList_Size(data_raw);
        this->p_data = (int *)malloc(sizeof(int) * size); 
        for (int i = 0; i < size; ++i) {
            *(this->p_data + i) = (int)PyInt_AsLong( PyList_GetItem(data_raw, i) );
        }
    }
    ~Matrix() {
        if (this->p_data != NULL) {
            free(this->p_data);
            this->p_data = NULL;
        }
    }

    int length() {
        return this->row;
    }

    int getRange() {
        return this->span;
    }

    int at(int x, int y) {
        int idx = 0;
        idx = this->span * x + y;
        if (idx < this->size) {
            return *(p_data + idx);
        } else {
            return 0;
        }
    }

    int sumOfCol(int c) {
        float sum = 0.0;
        int last = c;
        for (int i = 0; i < this->col; ++i) {
            sum += *(p_data + last);
            last += this->span;
        }
        return sum;
    }

    int in(int x, int y);

private:
    int row;
    int col;
    int span;
    int size;

    int* p_data;
};



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
    
};

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


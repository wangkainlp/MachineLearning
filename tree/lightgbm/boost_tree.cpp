#include <Python.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>

#include "common.h"
#include "util.h"
#include "data.h"
#include "tree_node.h"
#include "sample.h"
// #include "xgboost_tree.h"


using namespace std;

#define FLT_DIG         6                       /* # of decimal digits of precision */
#define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */
#define FLT_GUARD       0
#define FLT_MANT_DIG    24                      /* # of bits in mantissa */
#define FLT_MAX         3.402823466e+38F        /* max value */
#define FLT_MAX_10_EXP  38                      /* max decimal exponent */
#define FLT_MAX_EXP     128                     /* max binary exponent */
#define FLT_MIN         1.175494351e-38F        /* min positive value */
#define FLT_MIN_10_EXP  (-37)                   /* min decimal exponent */
#define FLT_MIN_EXP     (-125)                  /* min binary exponent */
#define FLT_NORMALIZE   0
#define FLT_RADIX       2                       /* exponent radix */
#define FLT_ROUNDS      1                       /* addition rounding: near */

#define LAMBDA  trainConf.lambda // 

/*
#define MAX(a, b)   (a > b ? a : b)
#define EQUAL(a, b) ( (a - b) > 0 ? (a - b) < 1e-7f : (b - a) < 1e-7f )
#define LESS(a, b) ( a + 1e-7f < b)
#define LARGER(a, b) ( b + 1e-7f < a)
// #define LAMBDA  0.01 // 

// global param
static PyObject *TreeError;


TrainConf trainConf;

int gFScore[1000];
float gGain[1000];
float gCover[1000];
*/


static PyObject *TreeError; 



int comp(const pair<int, float>& a, const pair<int, float>& b) {
    return a.second < b.second;
}

void greedyFindBin(vector<double> distinctValues, vector<int> counts, int maxBin, int minDataInBin) {

    vector<double> bin_upper_bound;
    if (distinctValues.size() <= maxBin) {
        int cnt = 0;
        for (size_t i = 0; i < distinctValues.size() - 1; ++i) {
            cnt += counts[i];
            if (cnt >= minDataInBin) {
                int val = (distinctValues[i] + distinctValues[i + 1]) / 2.0;
                if (bin_upper_bound.empty() && !DoubleEqual(bin_upper_bound.back(), val)) {
                    bin_upper_bound.push_back(val);
                    cnt = 0;
                }
            }
        }
        bin_upper_bound.push_back(std::numeric_limits<double>::infinity());
    } else {
        int totalSize = 0;
        for (size_t i = 0; i < distinctValues.size(); ++i) {
            totalSize += counts[i];
        }
        int meanBinSize = (int)(totalSize / distinctValues.size());

        vector<bool> needOneBin(distinctValues.size());
        for (size_t i = 0; i < distinctValues.size(); ++i) {
            if (distinctValues[i] >= meanBinSize()) {
                needOneBin[i] = true;
            } else {
                neadOneBin[i] = false;
            }
        }

        vector<double> binUpperBound(maxBin);
        vector<double> binLowerBound(maxBin);
        int inBinCnt = 0;
        int binCnt = 0;
        for (size_t i = 0; i < distinctValues.size(); ++i) {
            inBinCnt += counts[i];
            if (inBinCnt >= maxBin || needOneBin[i] || (needOneBin[i + 1] && inBinCnt >= (meanBinSize / 2))) {
                binUpperBound[binCnt] = distinctValues[i];
                ++binCnt;
                binLowerBound[binCnt] = distinctValues[i];
            }
        }
    }


}

static TreeNode* lightgbmLoop(Matrix& data, vector<float>& predVec, TreeNode* root, vector<int>& itemVec, vector<int>& feaVec) {


    time_t begin = time(NULL);
    printf("depth: %d\n", root->depth);
    // /*
    printf("items: ");
    for (size_t i = 0; i < itemVec.size(); ++i) {
        printf("%d ", itemVec[i]);
    }
    printf("\n");
    // */

    TreeNode* newNode = new TreeNode(); 
    newNode->father = root;
    newNode->size = (int)itemVec.size();

    newNode->itemIds = (int*)malloc(sizeof(int) * newNode->size);
    for (int _i = 0; _i < newNode->size; ++_i) {
        newNode->itemIds[_i] = itemVec[_i];
    }

    /*
    float sum = 0.0;
    for (int i = 0; i < itemVec.size(); ++i) {
        sum += data.at(itemVec[i], data.getRange()-1);
    }
    */

    float G  = 0.0f, H  = 0.0f;
    for (int j = 0; j < newNode->size; ++j) {
        int _id = newNode->itemIds[j];
        float pred  = predVec[_id];
        float label = data.at(_id, data.getRange() - 1);
        float g = pred - label;
        float h = pred * (1.0f - pred);
        G += g;
        H += h;
    }

    float lr = trainConf.shrink;
    newNode->value = -(G / (H + LAMBDA)) * lr;
    //float w = -(G / (H + LAMBDA)) * lr;
    // newNode->value = 1.0f / (1.0f + exp(-1.0f * w));
    newNode->depth = root->depth + 1;

    // 最大深度
    int maxDepth = trainConf.maxDepth;
    if (newNode->depth >= maxDepth) {

    if (abs(newNode->value) > 1) {
        printf("value:%f G:%f H:%f\n", newNode->value, G, H);
        for (int j = 0; j < newNode->size; ++j) {
            int _id = newNode->itemIds[j];
            float pred  = predVec[_id];
            float label = data.at(_id, data.getRange() - 1);
            printf("pred:%f label:%f\n", pred, label);
            printf("g:%f h:%f\n", pred - label, pred * (1.0 - pred));
        }
    }

        return newNode;
    }

    // 节点覆盖的最小数据量
    // int minCoverNum = 50;
    // int minCoverNum = 1;
    int minCoverNum = trainConf.minCoverNum;
    if (newNode->size <= minCoverNum) {

        if (abs(newNode->value) > 1) {
            printf("value:%f G:%f H:%f\n", newNode->value, G, H);
            for (int j = 0; j < newNode->size; ++j) {
                int _id = newNode->itemIds[j];
                float pred  = predVec[_id];
                float label = data.at(_id, data.getRange() - 1);
                printf("pred:%f label:%f\n", pred, label);
                printf("g:%f h:%f\n", pred - label, pred * (1.0 - pred));
            }
        }


        return newNode;
    }

    int minFea = -1;
    float minSp = FLT_MAX;
    float minVar = FLT_MAX;

    // printf("loop1:%ld\n", time(NULL) - begin);

    // findBestSpV2(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
    // findBestSpV1(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
    // findBestSp(data, sortIdMap, itemVec, feaVec, minFea, minSp, minVar);
    // printf("minFea: %d %.3f %.3f\n", minFea, minSp, minVar);

    // printf("loop2:%ld\n", time(NULL) - begin);

    // 没有分裂点
    // if (minFea >= FLT_MAX) {
    float minGain = trainConf.minGain;
    if (minFea < 0 or minVar < minGain) {

        if (abs(newNode->value) > 1) {
            printf("value:%f G:%f H:%f\n", newNode->value, G, H);
            for (int j = 0; j < newNode->size; ++j) {
                int _id = newNode->itemIds[j];
                float pred  = predVec[_id];
                float label = data.at(_id, data.getRange() - 1);
                printf("pred:%f label:%f\n", pred, label);
                printf("g:%f h:%f\n", pred - label, pred * (1.0 - pred));
            }
        }

        // newNode->itemIds = NULL;
        return newNode;
    }

    gFScore[minFea] += 1;
    gGain[minFea]   += minVar;
    gCover[minFea]  += H;

    newNode->fidx = minFea;
    newNode->split = minSp;

    vector<int> leftItems;
    vector<int> rightItems;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        // if (data.at(itemVec[i], minFea) <= minSp) {
        if (data.at(itemVec[i], minFea) < minSp) {
            leftItems.push_back(itemVec[i]);
        } else {
            rightItems.push_back(itemVec[i]);
        }
    }

    // 无法继续划分
    if (leftItems.size() <= 0 || rightItems.size() <= 0) {
        printf("左子树或右子树为空: %d\t%f\t%f\n", minFea, minSp, minVar);
    }

    newNode->left  = lightgbmLoop(data, sortIdMap, predVec, newNode, leftItems, feaVec);
    newNode->right = lightgbmLoop(data, sortIdMap, predVec, newNode, rightItems, feaVec);

    return newNode;
}



}

static TreeNode* lightgbm_main(Matrix& data, vector<float>& predVec, vector<int>& itemVec, vector<int>& feaVec) {

    // 预排序
    map<int, vector<int> > sortIdMap;

    for (size_t _i = 0; _i < feaVec.size(); ++_i) {
        int fea = feaVec[_i];

        vector<pair<int, float> > pairIdFeaVec;
        for (int i = 0; i < data.length(); ++i) {
            pairIdFeaVec.push_back(make_pair(i, data.at(i, fea)));
        }

        sort(pairIdFeaVec.begin(), pairIdFeaVec.end(), comp);

        vector<int> sortIdVec;
        for (size_t j = 0; j < pairIdFeaVec.size(); ++j) {
            sortIdVec.push_back(pairIdFeaVec[j].first);
        }
        sortIdMap[fea] = sortIdVec;
    }
 
    // 建树
    TreeNode* tmpRoot = new TreeNode();
    tmpRoot->depth = 0;
    TreeNode* root = lightgbmLoop(data, sortIdMap, predVec, tmpRoot, itemVec, feaVec);
    root->father = NULL;

    printTree(root);

    return root;
}


static PyObject * tree_lightgbm(PyObject *self, PyObject *args) {
    PyObject* data_raw;
    PyObject* train_conf_raw;
    if (!PyArg_ParseTuple(args, "OO", &data_raw, &train_conf_raw)) {
        return NULL;
    }

    // 解析配置
    trainConf.init(train_conf_raw);
    printf("row:%d col:%d shrink:%f\n", trainConf.rowWidth, trainConf.colWidth, trainConf.shrink);
    printf("data_size:%d fea_dim:%d\n", trainConf.dataSize, trainConf.dimSize);
    printf("row_rate:%f col_rate:%f\n", trainConf.rowSample, trainConf.colSample);
    printf("maxDepth:%d minCoverNum:%d\n", trainConf.maxDepth, trainConf.minCoverNum);
    printf("rand seed:%d\n", trainConf.randSeed);

    // 随机数种子
    srand(trainConf.randSeed);

    // 解析数据
    Matrix data(data_raw, trainConf.rowWidth, trainConf.colWidth);

    printf("check data:%f\n", data.in(trainConf.rowWidth - 1, trainConf.colWidth - 1)); 

    vector<float> predVec;
    predVec.resize(trainConf.rowWidth);
    for (size_t i = 0; i < trainConf.rowWidth; ++i) {
        predVec[i] = 0.5;
    }

    printf("run 1\n");

    vector<TreeNode*> trees;

    for (int i = 0; i < trainConf.treeSize; ++i) {
        printf("run 1.0\n");
        // 采样
        vector<int> itemVec = rowSample(trainConf.dataSize, trainConf.rowSample);
        vector<int> feaVec  = columnSample(trainConf.dimSize, trainConf.colSample);

        printf("run 2\n");
        
        // 建树
        TreeNode* root = lightgbm_main(data, predVec, itemVec, feaVec);

        printf("run 3\n");
        trees.push_back(root); 
        int dim = trainConf.dimSize;
        for (size_t j = 0; j < trainConf.dataSize; ++j) {
            float feas[dim];
            for (size_t k = 0; k < dim; ++k) {
                feas[k] = data.at(j, k);
            }
            predVec[j] = boostPredict(trees, feas, 0.0);
            predVec[j] = predTransform(predVec[j]);
        }
        printf("run 3.4\n");
    }

    /*
    // build输出数组
    PyObject *pArgs = PyTuple_New(tree_size);
    for (int i = 0; i < tree_size; ++i) {
        PyObject* newNode = newTreeNode(tree[i]);
        PyTuple_SetItem(pArgs, i, newNode);
    }
    */

    PyObject *pArgs = PyTuple_New(trainConf.treeSize);
    for (int idx = 0; idx < trainConf.treeSize; ++idx) {
        // 二叉树转换成一维数据表示
        // int tree_size = pow(2, 5) - 1;
        int tree_size = pow(2, trainConf.maxDepth) - 1;

        TreeNode* tree[tree_size];
        for (int i = 0; i < tree_size; ++i) {
            tree[i] = NULL;
        }

        // tree2array(root, tree, tree_size);
        tree2array(trees[idx], tree, tree_size);

        PyObject *pTree = PyTuple_New(tree_size);
        for (int i = 0; i < tree_size; ++i) {
            PyObject* newNode = newTreeNode(tree[i]);
            PyTuple_SetItem(pTree, i, newNode);
        }
        PyTuple_SetItem(pArgs, idx, pTree);
    }
    return Py_BuildValue("O", pArgs);
}


static PyMethodDef TreeMethods[] = {
    {"lightgbm",  tree_lightgbm,  METH_VARARGS, "lightgbm main"},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC inittree(void) {
    PyObject *m;

    m = Py_InitModule("tree", TreeMethods);

    if (m == NULL) {
        return;
    }

    TreeError = PyErr_NewException((char *)"tree.error", NULL, NULL);

    Py_INCREF(TreeError);

    PyModule_AddObject(m, "error", TreeError);

}


int main(int argc, char *argv[]) {
    // Pass argv[0] to the Python interpreter
    Py_SetProgramName(argv[0]);

    // Initialize the Python interpreter
    Py_Initialize();

    // Add a static module
    inittree();
}

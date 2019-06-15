#include <Python.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include <pthread.h>

#include "myutil.h"

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

/*
minCoverNum = 50
maxDepth = 5
maxLeaves = 10
*/

static PyObject *TreeError;

// 多线程参数结构体
struct Param {
    Matrix* data_ptr;
    vector<int>* itemVec;
    map<int, vector<int> >* sortIdMap;
    int fea;
    float minSp;
    float minVar;
};


struct timeval tv;

static double getTime() {
    gettimeofday(&tv, NULL);
    double mm = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
    return mm;
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
        cnt += 1;
        printf("%d: ", cnt);
        std::vector<TreeNode * > newlevel;
        for (int i = 0; i < level.size(); ++i) {
            if (level[i]->left != NULL) {
                newlevel.push_back(level[i]->left);
            }
            if (level[i]->right!= NULL) {
                newlevel.push_back(level[i]->right);
            }
            printf("%d:%.2f ", level[i]->fidx, level[i]->split);
        }
        printf("\n");
        level.clear();
        for (int i = 0; i < newlevel.size(); ++i) {
            level.push_back(newlevel[i]);
        }
    }
}


static float dataVariance(float sp, float feaList[], float labelList[], int len) {
    int   lSize = 0;
    float lSum = 0.0;
    int   rSize = 0;
    float rSum = 0.0;

    int i;
    for (i = 0; i < len; ++i) {
        if (feaList[i] <= sp) {
            ++lSize;
            lSum += labelList[i];
        } else {
            ++rSize;
            rSum += labelList[i];
        }
    }
    float var = -1.0 * ( (lSum / lSize * lSum) + (rSum / rSize * rSum) );
    return var;
}

static void findLeastVar(vector<float>& spList, float feaList[], float labelList[], int len,
                         float& minSp, float& minVar) {
    minSp = FLT_MAX;
    minVar = FLT_MAX;
    int spLen = spList.size();
    for (int i = 0; i < spLen; ++i) {
        float var = dataVariance(spList[i], feaList, labelList, len);
        if (var < minVar) {
            minVar = var;
            minSp = spList[i];
        }
    }
}

static void findLeastVarV1(vector<float>& spList, float feaList[], float labelList[], int len,
                         float& minSp, float& minVar) {
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    float sum = 0.0;
    for (int i = 0; i < len; ++i) {
        sum += labelList[i];
    }

    int spLen = spList.size();

    int spIdx = 0;
    float _sum = 0.0;
    for (int i = 1; i < len; ++i) {
        if (spIdx > spLen) {
            break;
        }
        _sum += labelList[i - 1];
        if (feaList[i - 1] < spList[spIdx] && spList[spIdx] < feaList[i] ) {
            float rSum = sum - _sum;
            float var = -1 * ( (_sum / i * _sum) + (rSum / (len - i) * rSum) );
            if (var < minVar) {
                minSp = spList[spIdx];
                minVar = var;
                // ++spIdx;
            }
            ++spIdx;
        }

    }
}


void* thread_fea(void* param_ptr) {
    struct Param* param = (struct Param*)param_ptr;


    int fea = param->fea;
    int dataLen = param->data_ptr->length();
    int rangeSize = param->data_ptr->getRange();
    vector<int> itemVec = *(param->itemVec);
    int itemSize = itemVec.size();

    int mask[dataLen];
    for (int j = 0; j < dataLen; ++j) { mask[j] = 0; }
    for (int j = 0; j < itemSize; ++j) { mask[itemVec[j]] += 1; }

    vector<int> sortId = (*(param->sortIdMap))[fea];

    int _sortId[dataLen];
    for (int j = 0; j < dataLen; ++j) {
        _sortId[ sortId[j] ] = j;
    }


    float feaDataVec[itemSize];
    float labelDataVec[itemSize];
    int _idx = 0;
    for (int j = 0; j < dataLen; ++j) {
        int id = _sortId[j];
        // if (mask[id] > 0) {
        for (int k = 0; k < mask[id]; ++k) {
            feaDataVec[_idx] = param->data_ptr->at(id, fea);
            labelDataVec[_idx] = param->data_ptr->at(id, rangeSize - 1);
            ++_idx;
        }
    }

    vector<float> spVec;
    for (int j = 1; j < itemSize; ++j) {
        if (feaDataVec[j - 1] + 1e-7 < feaDataVec[j]) {
            spVec.push_back(0.5 * (feaDataVec[j - 1] + feaDataVec[j]));
        }
    }


    if (spVec.size() <= 0) {
        printf("fea:%d spVec is none\n", fea);
        return (void*)0;
    }

    float curSp = FLT_MAX;
    float curVar = FLT_MAX;

    findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);


    param->minSp = curSp;
    param->minVar = curVar;

    return (void*)0;
}

static void findBestSpV2(Matrix& data, map<int, vector<int> >& sortIdMap,
                         vector<int>& itemVec, vector<int>& feaVec,
                         int& minFea, float& minSp, float& minVar) {

    int threadSize = 5;
    int step = (feaVec.size() + threadSize - 1) / threadSize;

    struct Param params[feaVec.size()];
    for (int j = 0; j < feaVec.size(); ++j) {
        params[j].data_ptr = &data;
        params[j].itemVec  = &itemVec;
        params[j].sortIdMap = &sortIdMap;
        params[j].fea = feaVec[j];
        params[j].minSp  = FLT_MAX;
        params[j].minVar = FLT_MAX;
    }

    int max_thread_size = 3;
    int thread_size = 0;
    for (int i = 0; i < feaVec.size(); i += thread_size) {
        // int fea = feaVec[i];

        thread_size = max_thread_size;
        if (i + max_thread_size > feaVec.size()) {
            thread_size = feaVec.size() - i;
        }

        pthread_t thread_pool[thread_size];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize (&attr, 16*1024*1024);    //修改栈大小为16M
        for (int j = 0; j < thread_size; ++j) {
            int ret = pthread_create(&thread_pool[j], &attr, thread_fea, (void*)&params[i + j]);
            if (ret) {
                printf("thread create error!\n");
            }
        }

        for (int j = 0; j < thread_size; ++j) {
            // void* status;
            // pthread_join(thread_pool[j], &status);
            pthread_join(thread_pool[j], NULL);
        }
    }


    minFea = -1;
    minSp = FLT_MAX;
    minVar = FLT_MAX;
    for (int j = 0; j < feaVec.size(); ++j) {
        if (params[j].minVar < minVar) {
            printf("curVar:%f minVar:%f\n", params[j].minVar, minVar);
            minFea = params[j].fea;
            minSp = params[j].minSp;
            minVar = params[j].minVar;
        }
    }
    printf("minfea: %d %.3f %.3f\n", minFea, minSp, minVar);

}




static void findBestSpV1(Matrix& data, map<int, vector<int> >& sortIdMap,
                         vector<int>& itemVec, vector<int>& feaVec,
                         int& minFea, float& minSp, float& minVar) {
    minFea = -1;
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    int rangeSize = data.getRange();
    int dataLen   = data.length();
    int itemSize  = itemVec.size();

    for (int i = 0; i < feaVec.size(); ++i) {

        // printf("run 1\n");
        // time_t feaBegin = time(NULL);
        double feaBegin = getTime();

        int fea = feaVec[i];

        int mask[dataLen];
        for (int j = 0; j < dataLen; ++j) { mask[j] = 0; }
        for (int j = 0; j < itemSize; ++j)  { mask[itemVec[j]] += 1; }

        // printf("\n");
        /*
        printf("mask: ");
        for (int x = 0; x < dataLen; ++x)
            printf("%d ", mask[x]);
        printf("\n");
        */

        // printf("run 2\n");
        vector<int> sortId = sortIdMap[fea];
        /*
        printf("sordId: ");
        for (int x = 0; x < sortId.size(); ++x)
            printf("%d ", sortId[x]);
        printf("\n");
        */

        // printf("feaLoop 1:%lf\n", getTime() - feaBegin);

        float _feaSortVec[dataLen];
        float _labelSortVec[dataLen];
        int   _feaSortCntVec[dataLen];
        for (int j = 0; j < dataLen; ++j) { _feaSortCntVec[j] = 0; }
        // printf("run 2.1\n");

        // printf("_mask:");
        // printf("%d:\n", fea);
        for (int j = 0; j < itemSize; ++j) {
            int idx = itemVec[j];
            // printf("%d:%d:%d ", idx, mask[idx], sortId[idx]);
            if (mask[idx] > 0) {
                _feaSortVec[ sortId[idx] ] = data.at(idx, fea);
                _feaSortCntVec[ sortId[idx] ] = mask[idx];

                _labelSortVec[ sortId[idx] ] = data.at(idx, rangeSize - 1);
            }
            // printf("%d ", mask[idx]);
        }
        // printf("run 3\n");

        // printf("\n");

        /*
        printf("feaSort: ");
        for (int j = 0; j < dataLen; ++j) {
            printf("%.3lf ", _feaSortVec[j]);
        }
        printf("\n");
        */

        /*
        printf("cnt: ");
        for (int j = 0; j < dataLen; ++j) {
            printf("%d ", _feaSortCntVec[j]);
        }
        printf("\n");
        */


        // 特征值列表排序
        int itemSize = itemVec.size();
        float labelDataVec[itemSize];
        float feaDataVec[itemSize];
        int _idx = 0;

        vector<float> feaSortVec;
        for (int j = 0; j < dataLen; ++j) {
            for (int k = 0; k < _feaSortCntVec[j]; ++k) {
                feaSortVec.push_back(_feaSortVec[j]);
                // printf("%.3f ", _feaSortVec[j]);
                
                feaDataVec[_idx] = _feaSortVec[j];
                labelDataVec[_idx] = _labelSortVec[j];
                ++_idx;
            }
        }
        /*
        printf("feaSortVec: \n");
        for (int j = 0; j < feaSortVec.size(); ++j) {
            printf("%.3f ", feaSortVec[j]);
        }
        printf("\n");
        */
        // printf("run 4\n");
        // printf("feaLoop 2:%lf\n", getTime() - feaBegin);
        // find min sp
        vector<float> spVec;
        for (int j = 1; j < feaSortVec.size(); ++j) {
            if (feaSortVec[j - 1] + 1e-7 < feaSortVec[j]) {
                spVec.push_back(0.5 * (feaSortVec[j - 1] + feaSortVec[j]));
            }
        }

        if (spVec.size() <= 0) {
            printf("fea:%d spVec is none\n", fea);
            continue;
        }

        // printf("feaLoop 3:%lf\n", getTime() - feaBegin);

        /*
        int itemSize = itemVec.size();

        float labelDataVec[itemSize];
        float feaDataVec[itemSize];
        for (int j = 0; j < itemSize; ++j) {
            feaDataVec[j] = data.at(itemVec[j], fea);
            labelDataVec[j] = data.at(itemVec[j], data.getRange() - 1);
        }
        printf("feaLoop 4:%lf\n", getTime() - feaBegin);
        */

        /*
        printf("fea sort:  ");
        for (int j = 0; j < itemSize; ++j) {
            printf("%.3f ", feaDataVec[j]);
        }
        printf("\n");

        printf("label sort:");
        for (int j = 0; j < itemSize; ++j) {
            printf("%.3f ", labelDataVec[j]);
        }
        printf("\n");
        */
        // printf("run 5\n");

        float curSp = FLT_MAX;
        float curVar = FLT_MAX;
        // findLeastVar(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);
        findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);

        if (curVar < minVar) {
            printf("curVar:%f minVar:%f\n", curVar, minVar);
            minFea = fea;
            minSp  = curSp;
            minVar = curVar;
        }
        // printf("feaLoop 5:%lf\n", time(NULL) - feaBegin);
        // printf("data size:%d sp size:%ld\n", itemSize, spVec.size());
        // printf("curfea: %d %.3f %.3f\n", fea, curSp, curVar);
        //
        // printf("feaLoop time:%lf\n", time(NULL) - feaBegin);
    }
    printf("minfea: %d %.3f %.3f\n", minFea, minSp, minVar);
}

static void findBestSp(Matrix& data, map<int, vector<int> >& sortIdMap,
                       vector<int>& itemVec, vector<int>& feaVec,
                       int& minFea, float& minSp, float& minVar) {
    minFea = -1;
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    int rangeSize = data.getRange();
    int dataLen = data.length();
    int itemSize = itemVec.size();

    for (int i = 0; i < feaVec.size(); ++i) {

        // time_t feaBegin = time(NULL);
        double feaBegin = getTime();

        int fea = feaVec[i];

        int mask[dataLen];
        for (int j = 0; j < dataLen; ++j) { mask[j] = 0; }
        for (int j = 0; j < itemSize; ++j)  { mask[itemVec[j]] += 1; }

        // printf("\n");
        /*
        printf("mask: ");
        for (int x = 0; x < dataLen; ++x)
            printf("%d ", mask[x]);
        printf("\n");
        */

        vector<int> sortId = sortIdMap[fea];
        /*
        printf("sordId: ");
        for (int x = 0; x < sortId.size(); ++x)
            printf("%d ", sortId[x]);
        printf("\n");
        */

        // printf("feaLoop 1:%lf\n", getTime() - feaBegin);

        float _feaSortVec[dataLen];
        float _labelSortVec[dataLen];
        int   _feaSortCntVec[dataLen];
        for (int j = 0; j < dataLen; ++j) { _feaSortCntVec[j] = 0; }

        // printf("_mask:");
        for (int j = 0; j < itemSize; ++j) {
            int idx = itemVec[j];
            // printf("%d:%d:%d ", idx, mask[idx], sortId[idx]);
            if (mask[idx] > 0) {
                _feaSortVec[ sortId[idx] ] = data.at(idx, fea);
                _feaSortCntVec[ sortId[idx] ] = mask[idx];

                _labelSortVec[ sortId[idx] ] = data.at(idx, rangeSize - 1);
            }
            // printf("%d ", mask[idx]);
        }
        // printf("\n");

        /*
        printf("feaSort: ");
        for (int j = 0; j < dataLen; ++j) {
            printf("%.3lf ", _feaSortVec[j]);
        }
        printf("\n");
        */

        /*
        printf("cnt: ");
        for (int j = 0; j < dataLen; ++j) {
            printf("%d ", _feaSortCntVec[j]);
        }
        printf("\n");
        */


        // 特征值列表排序
        int itemSize = itemVec.size();
        float labelDataVec[itemSize];
        float feaDataVec[itemSize];
        int _idx = 0;

        vector<float> feaSortVec;
        for (int j = 0; j < dataLen; ++j) {
            for (int k = 0; k < _feaSortCntVec[j]; ++k) {
                feaSortVec.push_back(_feaSortVec[j]);
                // printf("%.3f ", _feaSortVec[j]);
                
                feaDataVec[_idx] = _feaSortVec[j];
                labelDataVec[_idx] = _labelSortVec[j];
                ++_idx;
            }
        }
        /*
        printf("feaSortVec: \n");
        for (int j = 0; j < feaSortVec.size(); ++j) {
            printf("%.3f ", feaSortVec[j]);
        }
        printf("\n");
        */
        // printf("feaLoop 2:%lf\n", getTime() - feaBegin);
        // find min sp
        vector<float> spVec;
        for (int j = 1; j < feaSortVec.size(); ++j) {
            if (feaSortVec[j - 1] + 1e-7 < feaSortVec[j]) {
                spVec.push_back(0.5 * (feaSortVec[j - 1] + feaSortVec[j]));
            }
        }

        if (spVec.size() <= 0) {
            printf("fea:%d spVec is none\n", fea);
            continue;
        }

        // printf("feaLoop 3:%lf\n", getTime() - feaBegin);

        /*
        int itemSize = itemVec.size();

        float labelDataVec[itemSize];
        float feaDataVec[itemSize];
        for (int j = 0; j < itemSize; ++j) {
            feaDataVec[j] = data.at(itemVec[j], fea);
            labelDataVec[j] = data.at(itemVec[j], data.getRange() - 1);
        }
        printf("feaLoop 4:%lf\n", getTime() - feaBegin);
        */

        /*
        printf("fea sort:  ");
        for (int j = 0; j < itemSize; ++j) {
            printf("%.3f ", feaDataVec[j]);
        }
        printf("\n");

        printf("label sort:");
        for (int j = 0; j < itemSize; ++j) {
            printf("%.3f ", labelDataVec[j]);
        }
        printf("\n");
        */

        float curSp = FLT_MAX;
        float curVar = FLT_MAX;
        // findLeastVar(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);
        findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);

        if (curVar < minVar) {
            minFea = fea;
            minSp  = curSp;
            minVar = curVar;
        }
        // printf("feaLoop 5:%lf\n", time(NULL) - feaBegin);
        // printf("data size:%d sp size:%ld\n", itemSize, spVec.size());
        printf("curfea: %d %.3f %.3f\n", fea, curSp, curVar);
        //
        printf("feaLoop time:%lf\n", time(NULL) - feaBegin);
    }
}



static TreeNode* cartLoop(Matrix& data, map<int, vector<int> >& sortIdMap, TreeNode* root,
                          vector<int>& itemVec, vector<int>& feaVec) {

    time_t begin = time(NULL);
    printf("depth: %d\n", root->depth);
    /*
    printf("items: ");
    for (int i = 0; i < itemVec.size(); ++i) {
        printf("%d ", itemVec[i]);
    }
    printf("\n");
    */

    TreeNode* newNode = new TreeNode(); 
    newNode->father = root;
    newNode->size = itemVec.size();

    newNode->itemIds = (int*)malloc(sizeof(int) * newNode->size);
    for (int _i = 0; _i < newNode->size; ++_i) {
        *(newNode->itemIds + _i) = itemVec[_i];
    }

    float sum = 0.0;
    for (int i = 0; i < itemVec.size(); ++i) {
        sum += data.at(itemVec[i], data.getRange()-1);
    }
    newNode->value = sum / itemVec.size();
    newNode->depth = root->depth + 1;

    // 节点覆盖的最小数据量
    // int minCoverNum = 50;
    int minCoverNum = 1;
    // int minCoverNum = 1;
    if (newNode->size <= minCoverNum) {
        // newNode->itemIds = NULL;
        return newNode;
    }

    int minFea = -1;
    float minSp = FLT_MAX;
    float minVar = FLT_MAX;

    // printf("loop1:%ld\n", time(NULL) - begin);

    findBestSpV2(data, sortIdMap, itemVec, feaVec, minFea, minSp, minVar);
    // findBestSpV1(data, sortIdMap, itemVec, feaVec, minFea, minSp, minVar);
    // findBestSp(data, sortIdMap, itemVec, feaVec, minFea, minSp, minVar);
    // printf("minFea: %d %.3f %.3f\n", minFea, minSp, minVar);

    // printf("loop2:%ld\n", time(NULL) - begin);

    // if (minFea >= FLT_MAX) {
    if (minFea < 0) {
        // newNode->itemIds = NULL;
        return newNode;
    }

    int maxDepth = 5;
    if (newNode->depth >= maxDepth) {
        // newNode->itemIds = NULL;
        return newNode;
    }

    newNode->fidx = minFea;
    newNode->split = minSp;

    vector<int> leftItems;
    vector<int> rightItems;

    for (int i = 0; i < itemVec.size(); ++i) {
        if (data.at(itemVec[i], minFea) <= minSp) {
            leftItems.push_back(itemVec[i]);
        } else {
            rightItems.push_back(itemVec[i]);
        }
    }

    // 无法继续划分
    if (leftItems.size() <= 0 || rightItems.size() <= 0) {
        printf("%d\t%f\t%f\n", minFea, minSp, minVar);
    }

    newNode->left  = cartLoop(data, sortIdMap, newNode, leftItems, feaVec);
    newNode->right = cartLoop(data, sortIdMap, newNode, rightItems, feaVec);

    return newNode;
}

int comp(const pair<int, float>& a, const pair<int, float>& b) {
    return a.second < b.second;
}

static TreeNode* cart_main(Matrix& data, vector<int>& itemVec, vector<int>& feaVec) {

    // int feaSize = feaVec.size();
    // int itemSize = itemVec.size();

    // 预排序
    map<int, vector<int> > sortIdMap;
    // for (auto fea : feaVec) {
    for (int _i = 0; _i < feaVec.size(); ++_i) {
        int fea = feaVec[_i];

        vector<pair<int, float> > pairIdFeaVec;
        for (int i = 0; i < data.length(); ++i) {
            pairIdFeaVec.push_back(make_pair(i, data.at(i, fea)));
        }

        /*
        for (int j = 0; j < pairIdFeaVec.size(); ++j) {
            printf("%d ", pairIdFeaVec[j].first);
        }
        printf("\n");
        */

        sort(pairIdFeaVec.begin(), pairIdFeaVec.end(), comp);

        /*
        for (int j = 0; j < pairIdFeaVec.size(); ++j) {
            printf("%d ", pairIdFeaVec[j].first);
        }
        printf("\n");
        */

        vector<int> sortIdVec;
        vector<int> _sortIdVec;
        for (int j = 0; j < pairIdFeaVec.size(); ++j) {
            sortIdVec.push_back(pairIdFeaVec[j].first);
            // printf("%d:%d,%f\n", fea, it.first, it.second);
            _sortIdVec.push_back(0);
        }
        for (int j = 0; j < pairIdFeaVec.size(); ++j) {
            _sortIdVec[ sortIdVec[j] ] = j;
        }

        /* printf("%d:\n ", fea);
        for (int j = 0; j < pairIdFeaVec.size(); ++j) {
            printf("%d:%d ", j, _sortIdVec[j]);
        }
        printf("\n"); */

        /*
        for (auto& it : pairIdFeaVec) {
            sortIdVec.push_back(it.first);
            // printf("%d:%d,%f\n", fea, it.first, it.second);
        }
        */
        // printf("\n");
        // sortIdMap[fea] = sortIdVec;
        sortIdMap[fea] = _sortIdVec;
    }

    /*
    for (int i = 0; i < sortIdMap.size(); ++i) {
        vector<int> tmp = sortIdMap[i];
        printf("%d: ", i);
        for (int j = 0; j < tmp.size(); ++j) {
            printf("%d ", tmp[j]);
        }
        printf("\n");
    }
    */
    
    TreeNode* tmpRoot = new TreeNode();
    tmpRoot->depth = 0;
    TreeNode* root = cartLoop(data, sortIdMap, tmpRoot, itemVec, feaVec);
    root->father = NULL;

    printTree(root);

    return root;
}




static PyObject * tree_testdata(PyObject *self, PyObject *args) {
    PyObject * data_raw;
    PyObject * item_vec_raw;
    PyObject * fea_vec_raw;
    int row_width;
    int col_width;

    if (!PyArg_ParseTuple(args, "OiiOO", &data_raw, &row_width, &col_width,
                          &item_vec_raw, &fea_vec_raw)) {
        return NULL;
    }

    Matrix data(data_raw, row_width, col_width);

    vector<int> itemVec;
    vector<int> feaVec;

    for (int i = 0; i < PyList_Size(item_vec_raw); ++i) {
        itemVec.push_back( (int)PyInt_AsLong( PyList_GetItem(item_vec_raw, i) ) );
    }

    for (int i = 0; i < PyList_Size(fea_vec_raw); ++i) {
        feaVec.push_back( (int)PyInt_AsLong( PyList_GetItem(fea_vec_raw, i) ) );
    }

    printf("data size:%d, fea dim:%d\n", itemVec.size(), feaVec.size());

    double begin = getTime();

    TreeNode* root = cart_main(data, itemVec, feaVec);

    printf("cart main:%lf\n", getTime() - begin);

    printf("%d\n", data.in(row_width - 1, col_width - 1)); 

    int tree_size = pow(2, 5) - 1;

    TreeNode* tree[tree_size];
    for (int i = 0; i < tree_size; ++i) {
        tree[i] = NULL;
    }

    tree2array(root, tree, tree_size);

    PyObject *pArgs = PyTuple_New(tree_size);
    for (int i = 0; i < tree_size; ++i) {
        PyObject* newNode = newTreeNode(tree[i]);
        PyTuple_SetItem(pArgs, i, newNode);
    }

    return Py_BuildValue("O", pArgs);

    // return Py_BuildValue("i", 1);
    // Py_RETURN_NONE;
}

static PyObject * tree_passback(PyObject *self, PyObject *args) {

    // int a[5];
    vector<int> a;
    for (int i = 0; i < 5; ++i) {
        // a[i] = i;
        a.push_back(i);
    }

    return Py_BuildValue("[i]", a[0]);
}

static PyMethodDef TreeMethods[] = {
    // {"system", tree_system,   METH_VARARGS, "Execute a shell command."},
    {"testdata", tree_testdata, METH_VARARGS, "test pass array"},
    {"passback", tree_passback, METH_VARARGS, "test pass back array"},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC inittree(void) {
    PyObject *m;

    m = Py_InitModule("tree", TreeMethods);

    if (m == NULL) {
        return;
    }

    TreeError = PyErr_NewException("tree.error", NULL, NULL);

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

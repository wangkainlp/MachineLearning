#include <Python.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>

#include "common.h"
#include "myutil.h"
#include "data.h"
#include "tree_node.h"
#include "sample.h"
#include "xgboost_tree.h"
#include "gbdt_tree.h"


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
    size_t spLen = spList.size();
    for (size_t i = 0; i < spLen; ++i) {
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

    size_t spLen = spList.size();

    int spIdx = 0;
    float _sum = 0.0;
    for (size_t i = 1; i < len; ++i) {
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
                ++spIdx;
            }
        }

    }
}

static void findBestSp(Matrix& data, map<int, vector<int> >& sortIdMap,
                       vector<int>& itemVec, vector<int>& feaVec,
                       int& minFea, float& minSp, float& minVar) {
    minFea = -1;
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    int rangeSize = data.getRange();
    int dataLen = data.length();
    int itemSize = (int)itemVec.size();

    for (size_t i = 0; i < feaVec.size(); ++i) {

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
        int itemSize = (int)itemVec.size();
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
        for (size_t j = 1; j < feaSortVec.size(); ++j) {
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

static void findBestSpV1(Matrix& data, map<int, vector<int> >& sortIdMap,
                         vector<float> predVec,
                         vector<int>& itemVec, vector<int>& feaVec,
                         int& maxFea, float& maxSp, float& maxScore) {
    maxFea = -1;
    maxSp = -1.0f * FLT_MAX;
    maxScore = -1.0f * FLT_MAX;

    int rangeSize = data.getRange();
    int dataLen   = data.length();
    int itemSize  = (int)itemVec.size();

    for (size_t i = 0; i < feaVec.size(); ++i) {

        // printf("run 1\n");
        double feaBegin = getTime();

        int fea = feaVec[i];

        vector<int> sortId = sortIdMap[fea];

        int mask[dataLen];
        for (int j = 0; j < dataLen; ++j) { mask[j] = 0; }
        for (int j = 0; j < itemSize; ++j)  { mask[itemVec[j]] += 1; }

        // 特征值列表排序
        int itemSize = (int)itemVec.size();
        float labelDataVec[itemSize];
        float predDataVec[itemSize];
        float feaDataVec[itemSize];
        vector<float> feaSortVec;
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            int idx = sortId[j];
            int cnt = mask[idx];
            for (int k = 0; k < cnt; ++k) {
                feaDataVec[_idx] = data.at(idx, fea);
                feaSortVec.push_back( data.at(idx, fea) );
                labelDataVec[_idx] = data.at(idx, rangeSize - 1); 
                predDataVec[_idx] = predVec[idx];
                ++_idx;
            }
        }

        /*
        int mask[dataLen];
        for (int j = 0; j < dataLen; ++j) { mask[j] = 0; }
        for (int j = 0; j < itemSize; ++j)  { mask[itemVec[j]] += 1; }

        vector<int> sortId = sortIdMap[fea];

        float _feaSortVec[dataLen];
        float _labelSortVec[dataLen];
        float _predSortVec[dataLen];
        int   _feaSortCntVec[dataLen];
        for (int j = 0; j < dataLen; ++j) { _feaSortCntVec[j] = 0; }

        for (int j = 0; j < itemSize; ++j) {
            int idx = itemVec[j];
            if (mask[idx] > 0) {
                _feaSortVec[ sortId[idx] ] = data.at(idx, fea);
                _feaSortCntVec[ sortId[idx] ] = mask[idx];

                _labelSortVec[ sortId[idx] ] = data.at(idx, rangeSize - 1);
                _predSortVec[ sortId[idx] ] = predVec[idx];
            }
        }

        // 特征值列表排序
        int itemSize = (int)itemVec.size();
        float labelDataVec[itemSize];
        float predDataVec[itemSize];
        float feaDataVec[itemSize];
        int _idx = 0;

        vector<float> feaSortVec;
        for (int j = 0; j < dataLen; ++j) {
            for (int k = 0; k < _feaSortCntVec[j]; ++k) {
                feaSortVec.push_back(_feaSortVec[j]);
                feaDataVec[_idx]   = _feaSortVec[j];

                labelDataVec[_idx] = _labelSortVec[j];
                predDataVec[_idx]  = _predSortVec[j];
                ++_idx;
            }
        }
        */

        vector<float> spVec;
        for (size_t j = 1; j < feaSortVec.size(); ++j) {
            if (feaSortVec[j - 1] + 1e-7f < feaSortVec[j]) {
                spVec.push_back(0.5 * (feaSortVec[j - 1] + feaSortVec[j]));
            }
        }

        if (spVec.size() <= 0) {
            printf("fea:%d spVec is none\n", fea);
            continue;
        }

        // 
        float eps = 1e-16f;
        float G  = 0.0f, H  = 0.0f;
        for (int j = 0; j < itemSize; ++j) {
            float pred  = predDataVec[j];
            float label = labelDataVec[j];
            float g = pred - label;
            float h = MAX(pred * (1.0f - pred), eps);
            G += g;
            H += h;
        }

        float GL = 0.0f, HL = 0.0f;
        float GR = 0.0f, HR = 0.0f;
        float curFeaMaxScore = -1.0f * FLT_MAX;
        float curFeaMaxSp    = -1.0f * FLT_MAX;

        int spIdx = 0;
        for (int j = 1; j < itemSize; ++j) {
            float pred  = predDataVec[j - 1];
            float label = labelDataVec[j - 1];

            // obj函数
            float g = pred - label;
            float h = MAX(pred * (1.0f - pred), eps);
            GL += g;
            HL += h;
            GR = G - GL;
            HR = H - HL;

            if (feaDataVec[j - 1] < spVec[spIdx] && spVec[spIdx] < feaDataVec[j]) {
                float curScore = (GL * GL) / (HL + LAMBDA) + (GR * GR) / (HR + LAMBDA)
                                 - (G * G) / (H + LAMBDA);
                printf("curFea:%d, curSp:%f, curScore:%f\n", fea, spVec[spIdx], curScore);
                if (curScore > curFeaMaxScore) {
                    curFeaMaxScore = curScore;
                    curFeaMaxSp = spVec[spIdx];
                }
                ++spIdx;
            }

        }

        printf("curFea:%d, curMaxScore:%f maxScore:%f\n", fea, curFeaMaxScore, maxScore);
        if (curFeaMaxScore > maxScore) {
            printf("curScore:%f maxScore:%f\n", curFeaMaxScore,maxScore);
            maxFea = fea;
            maxSp  = curFeaMaxSp;
            maxScore = curFeaMaxScore;
        }

    }
    printf("maxfea: %d %.3f %.3f\n", maxFea, maxSp, maxScore);
}

static void findBestSpV2(Matrix& data, map<int, vector<int> >& sortIdMap,
                         vector<float> predVec,
                         vector<int>& itemVec, vector<int>& feaVec,
                         int& maxFea, float& maxSp, float& maxScore) {
    maxFea = -1;
    maxSp = -1.0f * FLT_MAX;
    maxScore = -1.0f * FLT_MAX;

    int rangeSize = data.getRange();
    int dataLen   = data.length();
    int itemSize  = (int)itemVec.size();

    // comput G H 
    float eps = 1e-16f;
    float G  = 0.0f, H  = 0.0f;
    for (int j = 0; j < itemSize; ++j) {
        float pred  = predVec[j];
        float label = data.at(itemVec[j], rangeSize - 1);
        float g = pred - label;
        float h = MAX(pred * (1.0f - pred), eps);
        G += g;
        H += h;
    }

    for (size_t i = 0; i < feaVec.size(); ++i) {

        // printf("run 1\n");
        double feaBegin = getTime();

        int fea = feaVec[i];

        vector<int> sortId = sortIdMap[fea];

        int mask[dataLen];
        for (int j = 0; j < dataLen; ++j) { mask[j] = 0; }
        for (int j = 0; j < itemSize; ++j)  { mask[itemVec[j]] += 1; }

        // 特征值列表排序
        int itemSize = (int)itemVec.size();
        float labelDataVec[itemSize];
        float predDataVec[itemSize];
        float feaSortDataVec[itemSize];
        vector<float> feaSortVec;
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            int idx = sortId[j];
            int cnt = mask[idx];
            for (int k = 0; k < cnt; ++k) {
                feaSortDataVec[_idx] = data.at(idx, fea);
                labelDataVec[_idx] = data.at(idx, rangeSize - 1); 
                predDataVec[_idx] = predVec[idx];
                ++_idx;
            }
        }

        vector<float> spVec;
        for (size_t j = 1; j < itemSize; ++j) {
            if (feaSortDataVec[j - 1] + 1e-7f < feaSortDataVec[j]) {
                spVec.push_back(0.5 * (feaSortDataVec[j - 1] + feaSortDataVec[j]));
            }
        }

        if (spVec.size() <= 0) {
            printf("fea:%d spVec is none\n", fea);
            continue;
        }

        float GL = 0.0f, HL = 0.0f;
        float GR = 0.0f, HR = 0.0f;
        float curFeaMaxScore = -1.0f * FLT_MAX;
        float curFeaMaxSp    = -1.0f * FLT_MAX;

        int spIdx = 0;
        for (int j = 1; j < itemSize; ++j) {
            float pred  = predDataVec[j - 1];
            float label = labelDataVec[j - 1];

            // obj函数
            float g = pred - label;
            float h = MAX(pred * (1.0f - pred), eps);
            GL += g;
            HL += h;
            GR = G - GL;
            HR = H - HL;

            if (feaSortDataVec[j - 1] < spVec[spIdx] && spVec[spIdx] < feaSortDataVec[j]) {
                float curScore = (GL * GL) / (HL + LAMBDA) + (GR * GR) / (HR + LAMBDA)
                                 - (G * G) / (H + LAMBDA);
                printf("curFea:%d, curSp:%f, curScore:%f\n", fea, spVec[spIdx], curScore);
                if (curScore > curFeaMaxScore) {
                    curFeaMaxScore = curScore;
                    curFeaMaxSp = spVec[spIdx];
                    // curFeaMaxSp = feaSortDataVec[j];
                }
                ++spIdx;
            }
        }

        printf("curFea:%d, curMaxScore:%f maxScore:%f\n", fea, curFeaMaxScore, maxScore);
        if (curFeaMaxScore > maxScore) {
            printf("curScore:%f maxScore:%f\n", curFeaMaxScore,maxScore);
            maxFea = fea;
            maxSp  = curFeaMaxSp;
            maxScore = curFeaMaxScore;
        }

    }
    printf("maxfea: %d %.3f %.3f\n", maxFea, maxSp, maxScore);
}



static TreeNode* xgboostLoop(Matrix& data, map<int, vector<int> >& sortIdMap, vector<float>& predVec, TreeNode* root,
                          vector<int>& itemVec, vector<int>& feaVec) {

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

    findBestSpV2(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
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

    newNode->left  = xgboostLoop(data, sortIdMap, predVec, newNode, leftItems, feaVec);
    newNode->right = xgboostLoop(data, sortIdMap, predVec, newNode, rightItems, feaVec);

    return newNode;
}

int comp(const pair<int, float>& a, const pair<int, float>& b) {
    return a.second < b.second;
}

static TreeNode* xgboost_main(Matrix& data, vector<float>& predVec, vector<int>& itemVec, vector<int>& feaVec) {

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
 

    /*
    for (size_t _i = 0; _i < feaVec.size(); ++_i) {
        int fea = feaVec[_i];

        vector<pair<int, float> > pairIdFeaVec;
        for (int i = 0; i < data.length(); ++i) {
            pairIdFeaVec.push_back(make_pair(i, data.at(i, fea)));
        }

        sort(pairIdFeaVec.begin(), pairIdFeaVec.end(), comp);

        vector<int> sortIdVec;
        vector<int> _sortIdVec;
        for (size_t j = 0; j < pairIdFeaVec.size(); ++j) {
            sortIdVec.push_back(pairIdFeaVec[j].first);
            _sortIdVec.push_back(0);
        }
        for (size_t j = 0; j < pairIdFeaVec.size(); ++j) {
            _sortIdVec[ sortIdVec[j] ] = (int)j;
        }

        sortIdMap[fea] = _sortIdVec;
    }
    */

    // 建树
    TreeNode* tmpRoot = new TreeNode();
    tmpRoot->depth = 0;
    TreeNode* root = xgboostLoop(data, sortIdMap, predVec, tmpRoot, itemVec, feaVec);
    root->father = NULL;

    printTree(root);

    return root;
}



void preSort(Matrix& data, int feaSize,
             map<int, vector<int> >& sortIdMap,
             map<int, vector<int>*>& sortIdMap_) {
    double begin = getTime();
    // 预排序
    for (size_t _i = 0; _i < feaSize; ++_i) {
        int fea = _i;

        vector<pair<int, float> > pairIdFeaVec;
        for (int i = 0; i < data.length(); ++i) {
            pairIdFeaVec.push_back(make_pair(i, data.at(i, fea)));
        }

        sort(pairIdFeaVec.begin(), pairIdFeaVec.end(), _comp);

        vector<int> sortIdVec;
        vector<int>* sortIdVec_ = new vector<int>();
        for (size_t j = 0; j < pairIdFeaVec.size(); ++j) {
            sortIdVec.push_back(pairIdFeaVec[j].first);
            sortIdVec_->push_back(pairIdFeaVec[j].first);
        }
        sortIdMap[fea] = sortIdVec;
        sortIdMap_[fea] = sortIdVec_;
    }
    printf("presort:%lf\n", getTime() - begin);
}

int _comp_(const Block& a, const Block& b) {
    return a.fea < b.fea;
}

void preSort_(Matrix* data, int feaSize,
              map<int, vector<Block>*>* sortIdMap) {
    double begin = getTime();
    // 预排序
    for (size_t _i = 0; _i < feaSize; ++_i) {
        int fea = _i;

        vector<Block>* sortIdVec = new vector<Block>();
        for (int i = 0; i < data->length(); ++i) {
            struct Block tmp;
            tmp.id    = i;
            tmp.fea   = data->at(i, fea);
            tmp.label = data->at(i, data->getRange() - 1);
            sortIdVec->push_back(tmp);
        }

        sort(sortIdVec->begin(), sortIdVec->end(), _comp_);

        // sortIdMap->at(fea) = sortIdVec;
        sortIdMap->insert(make_pair(fea, sortIdVec));
    }
    printf("presort:%lf\n", getTime() - begin);
}


static PyObject * tree_gbdt(PyObject *self, PyObject *args) {
    PyObject* data_raw;
    PyObject* train_conf_raw;
    if (!PyArg_ParseTuple(args, "OO", &data_raw, &train_conf_raw)) {
        return NULL;
    }


    // 解析配置
    // TrainConf trainConf;
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
    Matrix* data_ = new Matrix(data_raw, trainConf.rowWidth, trainConf.colWidth);

    printf("check data:%f\n", data.in(trainConf.rowWidth - 1, trainConf.colWidth - 1)); 

    vector<float> predVec;
    predVec.resize(trainConf.rowWidth);
    for (size_t i = 0; i < trainConf.rowWidth; ++i) {
        // predVec[i] = 0.5;
        predVec[i] = data.at(i, trainConf.colWidth - 1);
    }

    // map<int, vector<int> > sortIdMap;
    // map<int, vector<int>* > sortIdMap_p;
    // preSort(data, trainConf.dimSize, sortIdMap, sortIdMap_p);

    map<int, vector<Block>*>* sortIdMap_p = new map<int, vector<Block>*>();
    preSort_(data_, trainConf.dimSize, sortIdMap_p);

    printf("run 1\n");

    // vector<TreeNode*> trees(trainConf.treeSize);
    vector<TreeNode*> trees;

    vector<float> innerPreds(trainConf.dataSize, 0.0f);

    for (int i = 0; i < trainConf.treeSize; ++i) {
        printf("run 1.0\n");
        // 采样
        vector<int> itemVec = rowSample(trainConf.dataSize, trainConf.rowSample);
        vector<int> feaVec  = columnSample(trainConf.dimSize, trainConf.colSample);
        int feaSize = 0;
        for (int _i = 0; _i < feaVec.size(); ++_i) {
            feaSize += feaVec[_i];
        }
        printf("main feaSize:%d\n", feaSize);

        printf("run 2\n");
        
        double begin = getTime();
        // 建树
        // TreeNode* root = gbdt_main(data, sortIdMap_p, predVec, itemVec, feaVec);
        TreeNode* root = gbdt_main(data_, sortIdMap_p, predVec, itemVec, feaVec);
        printf("gbdt main:%lf\n", getTime() - begin);

        printf("run 3\n");
        trees.push_back(root); 
        int dim = trainConf.dimSize;
        for (size_t j = 0; j < trainConf.dataSize; ++j) {
            float feas[dim];
            for (size_t k = 0; k < dim; ++k) {
                feas[k] = data.at(j, k);
            }
            /*
            // predVec[j] = boostPredict(trees, feas, 0.0);
            // predVec[j] = predTransform(predVec[j]);
            float pred = boostPredict(trees, feas, 0.0);
            pred = predTransform(pred);
            predVec[j] = data.at(j, trainConf.colWidth - 1) - pred;
            */

            float pred = predict(root, feas);
            if (j <= 0) {
                innerPreds[j] = 0.0f + pred;
            } else {
                innerPreds[j] = innerPreds[j] + pred;
            }
            pred = predTransform(pred);
            predVec[j] = data.at(j, trainConf.colWidth - 1) - pred;
        }

        printf("tree main:%lf\n", getTime() - begin);
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




static PyObject * tree_xgboost_gbm(PyObject *self, PyObject *args) {
    PyObject* data_raw;
    PyObject* train_conf_raw;
    if (!PyArg_ParseTuple(args, "OO", &data_raw, &train_conf_raw)) {
        return NULL;
    }


    // 解析配置
    // TrainConf trainConf;
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

    // vector<TreeNode*> trees(trainConf.treeSize);
    vector<TreeNode*> trees;

    for (int i = 0; i < trainConf.treeSize; ++i) {
        printf("run 1.0\n");
        // 采样
        vector<int> itemVec = rowSample(trainConf.dataSize, trainConf.rowSample);
        vector<int> feaVec  = columnSample(trainConf.dimSize, trainConf.colSample);

        printf("run 2\n");
        
        // 建树
        TreeNode* root = _xgboost_main(data, predVec, itemVec, feaVec);

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


static PyObject * tree_xgboost(PyObject *self, PyObject *args) {
    PyObject* data_raw;
    PyObject* item_vec_raw;
    PyObject* fea_vec_raw;
    PyObject* pred_vec_raw;
    PyObject* train_conf_raw;
    int row_width;
    int col_width;

    if (!PyArg_ParseTuple(args, "OOOOO", &data_raw, &train_conf_raw,
                          &pred_vec_raw, &item_vec_raw, &fea_vec_raw)) {
        return NULL;
    }

    // 解析配置
    // TrainConf trainConf;
    trainConf.init(train_conf_raw);
    printf("row:%d col:%d shrink:%f\n", trainConf.rowWidth, trainConf.colWidth, trainConf.shrink);
    printf("maxDepth:%d minCoverNum:%d\n", trainConf.maxDepth, trainConf.minCoverNum);

    // 解析数据
    Matrix data(data_raw, trainConf.rowWidth, trainConf.colWidth);

    // 解析行采样, 列采样
    vector<int> itemVec;
    vector<int> feaVec;
    vector<float> predVec;

    for (int i = 0; i < PyList_Size(pred_vec_raw); ++i) {
        predVec.push_back( (float)PyFloat_AsDouble( PyList_GetItem(pred_vec_raw, i) ) );
    }

    for (int i = 0; i < PyList_Size(item_vec_raw); ++i) {
        itemVec.push_back( (int)PyInt_AsLong( PyList_GetItem(item_vec_raw, i) ) );
    }

    for (int i = 0; i < PyList_Size(fea_vec_raw); ++i) {
        feaVec.push_back( (int)PyInt_AsLong( PyList_GetItem(fea_vec_raw, i) ) );
    }

    printf("data size:%d, fea dim:%d\n", trainConf.rowWidth, trainConf.colWidth - 2);


    // 建树
    TreeNode* root = xgboost_main(data, predVec, itemVec, feaVec);

    printf("%f\n", data.in(trainConf.rowWidth - 1, trainConf.colWidth - 1)); 

    // 二叉树转换成一维数据表示
    // int tree_size = pow(2, 5) - 1;
    int tree_size = pow(2, trainConf.maxDepth) - 1;

    TreeNode* tree[tree_size];
    for (int i = 0; i < tree_size; ++i) {
        tree[i] = NULL;
    }

    tree2array(root, tree, tree_size);

    // build输出数组
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
    {"gbdt",  tree_gbdt,  METH_VARARGS, "gbdt main"},
    {"xgboost",  tree_xgboost,  METH_VARARGS, "xgboost main"},
    {"xgboost_gbm",  tree_xgboost_gbm,  METH_VARARGS, "xgboost gbm main"},
    {"passback", tree_passback, METH_VARARGS, "test pass back array"},
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

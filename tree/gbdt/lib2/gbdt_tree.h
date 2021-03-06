#ifndef _gbdt_tree_h_
#define _gbdt_tree_h_

#include <pthread.h>
#include <vector>
#include <memory>
#include <map>
#include <cmath>
#include "tree_node.h"
#include "data.h"
#include "loss.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void _findBestSpV2_1_(Matrix& data, map<int, vector<int> >& sortIdMap,
                         vector<float> predVec,
                         vector<int>& itemVec, vector<int>& feaVec,
                         int& maxFea, float& maxSp, float& maxScore) {
    maxFea = -1;
    maxSp = -1.0f * FLT_MAX;
    maxScore = -1.0f * FLT_MAX;

    int rangeSize = data.getRange();
    int dataLen   = data.length();
    // int itemSize  = (int)itemVec.size();
    int itemSize  = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        itemSize += itemVec[i];
    }

    float eps = 1e-16f;
    float G  = 0.0f, H  = 0.0f;
    for (size_t j = 0; j < itemVec.size(); ++j) {
        for (int k = 0; k < itemVec[j]; ++k) {
            float pred  = predVec[j];
            float label = data.at(j, rangeSize - 1);
            G += pred - label;
            H += MAX(pred * (1.0f - pred), eps);
        }
    }

    for (size_t i = 0; i < feaVec.size(); ++i) {
        int fea = (int)i;
        if (feaVec[fea] < 1) {
            continue;
        }

        printf("run f 1\n");

        vector<int> sortId = sortIdMap[fea];

        vector<float> feaSortVec;
        for (size_t j = 0; j < dataLen; ++j) {
            int id = sortId[j];
            if (itemVec[id] > 0) {
                feaSortVec.push_back(data.at(id, fea));
            }
        }
        vector<float> spVec;
        for (size_t j = 1; j < feaSortVec.size(); ++j) {
            if (feaSortVec[j - 1] + 1e-7f < feaSortVec[j]) {
                spVec.push_back(0.5 * (feaSortVec[j - 1] + feaSortVec[j]));
                printf("%f ", spVec.at(spVec.size() - 1));
            }
        }
        printf("\n");

        printf("run f 2\n");
        float curFeaMaxScore = -FLT_MAX;
        float curFeaMaxSp    = -FLT_MAX;
        float GL = 0.0f, HL = 0.0f;
        float GR = 0.0f, HR = 0.0f;
        float last = -FLT_MAX;
        int spIdx = 0;
        for (size_t j = 0; j < dataLen; ++j) {
            int id = sortId[j];
            if (itemVec[id] <= 0) {
                continue;
            }
            // data.at(id, fea) >= spVec[spIdx]
            if (last < spVec[spIdx] && !(data.at(id, fea) < spVec[spIdx]) ) {
                GR = G - GL;
                HR = H - HL;
                float curScore = (GL * GL) / (HL + LAMBDA) + (GR * GR) / (HR + LAMBDA)
                                     - (G * G) / (H + LAMBDA);
                printf("curFea:%d, curSp:%f, curScore:%f\n", fea, spVec[spIdx], curScore);
                if (curScore > curFeaMaxScore) {
                    curFeaMaxScore = curScore;
                    curFeaMaxSp = spVec[spIdx];
                    // curFeaMaxSp = feaSortDataVec[j];
                }
                ++spIdx;
                last = data.at(id, fea);
            }

            for (size_t k = 0; k < itemVec[id]; ++k) {
                float pred  = predVec[id];
                float label = data.at(id, rangeSize - 1);
                GL += pred - label;
                HL += MAX(pred * (1.0f - pred), eps);
            }
        }

        printf("curFea:%d, curMaxScore:%f maxScore:%f\n", fea, curFeaMaxScore, maxScore);
        if (curFeaMaxScore > maxScore) {
            printf("curScore:%f maxScore:%f\n", curFeaMaxScore,maxScore);
            maxFea = fea;
            maxSp  = curFeaMaxSp;
            maxScore = curFeaMaxScore;
        }
        printf("run f n\n");
    }

    printf("maxfea: %d %.3f %.3f\n", maxFea, maxSp, maxScore);
}

void _findBestSpV2_(Matrix& data, map<int, vector<int> >& sortIdMap,
                         vector<float> predVec,
                         vector<int>& itemVec, vector<int>& feaVec,
                         int& maxFea, float& maxSp, float& maxScore) {
    maxFea = -1;
    maxSp = -1.0f * FLT_MAX;
    maxScore = -1.0f * FLT_MAX;

    int rangeSize = data.getRange();
    int dataLen   = data.length();
    // int itemSize  = (int)itemVec.size();
    int itemSize  = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        itemSize += itemVec[i];
    }

    // comput G H 
    /*
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
    */
    float eps = 1e-16f;
    float G  = 0.0f, H  = 0.0f;
    for (size_t j = 0; j < itemVec.size(); ++j) {
        for (int k = 0; k < itemVec[j]; ++k) {
            float pred  = predVec[j];
            float label = data.at(j, rangeSize - 1);
            float g = pred - label;
            float h = MAX(pred * (1.0f - pred), eps);
            G += g;
            H += h;
        }
    }

    for (size_t i = 0; i < feaVec.size(); ++i) {

        if (feaVec[i] != 1) {
            continue;
        }
        int fea = i;

        double feaBegin = getTime();


        vector<int> sortId = sortIdMap[fea];

        /*
        for (int j = 0; j < dataLen; ++j) {
            printf("%d:%.1f ", sortId[j], data.at(sortId[j], fea));
        }
        printf("run f 2\n");
        */

        // 特征值列表排序
        // int itemSize = (int)itemVec.size();
        float labelDataVec[itemSize];
        float predDataVec[itemSize];
        float feaSortDataVec[itemSize];
        // vector<float> feaSortVec;
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            int idx = sortId[j];
            int cnt = itemVec[idx];
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
                // printf("%f ", 0.5 * (feaSortDataVec[j - 1] + feaSortDataVec[j]));
                // printf("%.1f ", feaSortDataVec[j - 1]);
            }
        }
        // printf("\n");

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


TreeNode* _xgboostLoop_(Matrix& data, map<int, vector<int> >& sortIdMap, vector<float>& predVec, TreeNode* root,
                      vector<int>& itemVec, vector<int>& feaVec) {

    time_t begin = time(NULL);
    printf("depth: %d\n", root->depth);
    /*
    printf("items: ");
    for (size_t i = 0; i < itemVec.size(); ++i) {
        for (int j = 0; j < itemVec[i]; ++j) {
            printf("%d ", (int)i);
        }
    }
    printf("\n");
    */

    TreeNode* newNode = new TreeNode(); 
    newNode->father = root;
    newNode->size = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        newNode->size += itemVec[i];
    }

    /*
    newNode->itemIds = (int*)malloc(sizeof(int) * newNode->size);
    for (int _i = 0; _i < newNode->size; ++_i) {
        newNode->itemIds[_i] = itemVec[_i];
    }
    */
    newNode->itemIds = (int*)malloc(sizeof(int) * newNode->size);
    int idx = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        for (int j = 0; j < itemVec[i]; ++j) {
            newNode->itemIds[idx] = (int)i;
            ++idx;
        }
    }
    // printf("run _1\n");

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
    // printf("run _2\n");

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

    // printf("run _3\n");
    _findBestSpV2_1_(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
    // _findBestSpV2(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
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

    vector<int> leftItems(trainConf.dataSize);
    int leftSize = 0;
    vector<int> rightItems(trainConf.dataSize);
    int rightSize = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        // if (data.at(itemVec[i], minFea) <= minSp) {
        // if (data.at(itemVec[i], minFea) < minSp) {
        // if (itemVec[i] > 0 && data.at(i, minFea) < minSp) {
        //     leftItems.push_back(itemVec[i]);
        // } else {
        //     rightItems.push_back(itemVec[i]);
        // }
        leftItems[i] = 0;
        rightItems[i] = 0;
        if (itemVec[i] > 0 && data.at(i, minFea) < minSp) {
            leftItems[i] = itemVec[i];
            leftSize += itemVec[i];
        } else {
            rightItems[i] = itemVec[i];
            rightSize += itemVec[i];
        }
    }

    // 无法继续划分
    // if (leftItems.size() <= 0 || rightItems.size() <= 0) {
    if (leftSize <= 0 || rightSize <= 0) {
        printf("左子树或右子树为空: %d\t%f\t%f\n", minFea, minSp, minVar);
    }

    newNode->left  = _xgboostLoop_(data, sortIdMap, predVec, newNode, leftItems, feaVec);
    newNode->right = _xgboostLoop_(data, sortIdMap, predVec, newNode, rightItems, feaVec);

    return newNode;
}

/* ========================================================================================================= */

typedef struct StaInfo {
	float* feaImprove;
	int* feaSelect;
}StaInfo;

typedef struct Block {
    // float id;
    int   id;
    float fea;
    float label;
}Block;


struct Param {
    Matrix* data;
    // map<int, vector<int> >* sortIdMap;
    // map<int, vector<int>*>* sortIdMap;
    map<int, vector<Block>*>* sortIdMap;
    vector<float>* predVec;
    vector<int>* feaVec;
    int depth;
    int idx;
    int groupId;
    int step;
    vector<TreeNode*>* nodes;
    vector<TreeNode*>* newNodes;
};

struct ParamLevel {
    Matrix* data;
    map<int, vector<Block>*>* sortIdMap;
    map<int, Block*>* p_sortIdMap;
	map<int, int*>* p_sort2IdMap;
    vector<int>* itemVec;
    vector<int>* feaVec;
    vector<float>* predVec;
    int groupId;
    int step;
	int start;
	int end;
    vector<TreeNode*>* nodes;
    vector<int>* minFea;
    vector<float>* minSp;
    vector<float>* minVar;
    vector<float>* improve;

	float* feaDataVec;
	float* labelDataVec;
	float** _labelDataVec;
	float* spVec;
	int8_t* itemMask;

	StaInfo* staInfo;
};

TreeNode* constructTreeNodeV1(vector<int>& itemVec, Matrix* data, vector<float>* predVec) {
    TreeNode* newNode = new TreeNode();
    newNode->father = NULL;             ///
    newNode->depth = 0;                 ///
    newNode->left = NULL;
    newNode->right = NULL;

    newNode->size = itemVec.size();
    newNode->itemIds = (int*)malloc(sizeof(int) * newNode->size);
    float sum = 0.0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        int id = itemVec[i];
        newNode->itemIds[i] = id;
        sum += predVec->at(id);
    }

    newNode->value = sum / newNode->size;
    // printf("value:%f sum:%f size:%d\n", newNode->value, sum, newNode->size);

    return newNode;
}

TreeNode* constructTreeNode(vector<int>& itemVec, Matrix* data, vector<float>* predVec) {
    TreeNode* newNode = new TreeNode();
    newNode->father = NULL;             ///
    // newNode->size = itemVec.size();
    newNode->depth = 0;                 ///
    newNode->left = NULL;
    newNode->right = NULL;

    newNode->size = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        newNode->size += itemVec[i];
    }

    newNode->itemIds = (int*)malloc(sizeof(int) * newNode->size);
    int idx = 0;
    for (int i = 0; i < itemVec.size(); ++i) {
        for (int j = 0; j < itemVec[i]; ++j) {
            newNode->itemIds[idx] = (int)i;
            ++idx;
        }
    }

    float sum = 0.0;
    for (int i = 0; i < itemVec.size(); ++i) {
        for (int j = 0; j < itemVec[i]; ++j) {
            // sum += predVec[j];
            sum += predVec->at(i);
        }
    }
    newNode->value = sum / newNode->size;
    // printf("value:%f sum:%f size:%d\n", newNode->value, sum, newNode->size);

    return newNode;
}

static void gbdt_findLeastVarV1(vector<float>& spList, float feaList[], float labelList[], int len,
                         float& minSp, float& minVar) {
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    float sum = 0.0;
    for (int i = 0; i < len; ++i) {
        sum += labelList[i];
    }
    // printf("run 2.0.0.0.1\n");

    int spLen = spList.size();

    int spIdx = 0;
    float _sum = 0.0;
    for (int i = 1; i < len; ++i) {
        if (spIdx > spLen) {
            break;
        }
        // printf("run 2.0.0.0.2\n");
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
        // printf("run 2.0.0.0.3\n");

    }
}

static void gbdt_findLeastVarV3(float spList[], int spLen,
	                         	float feaList[], float labelList[], float** _labelList, Block block[], Block** _block, int len,
                                float& minSp, float& minVar, float& improve) {
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    float sum = 0.0;
    for (int i = 0; i < len; ++i) {
        // sum += labelList[i];
        // sum += *(_labelList[i]);
		// sum  += _block[i]->label;
		sum  += block[i].label;
    }

	float beforeVar = -1 * sum / len * sum;

    int spIdx = 0;
    float _sum = 0.0;
    for (int i = 1; i < len; ++i) {
        if (spIdx >= spLen) {
            break;
        }
        _sum += block[i - 1].label;
        // _sum += _block[i - 1]->label;
        // _sum += labelList[i - 1];
        // _sum += *(_labelList[i - 1]);
        // if (feaList[i - 1] < spList[spIdx] && spList[spIdx] < feaList[i] ) {
        // if (_block[i - 1]->fea < spList[spIdx] && spList[spIdx] < _block[i]->fea ) {
        if (block[i - 1].fea < spList[spIdx] && spList[spIdx] < block[i].fea ) {
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

	improve = beforeVar - minVar;
	// printf("improve:%lf\n", improve);
}






static void gbdt_findLeastVarV2(float spList[], int spLen,
	                         	float feaList[], float labelList[], float** _labelList, Block block[], Block** _block, int len,
                                float& minSp, float& minVar, float& improve) {
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    float sum = 0.0;
    for (int i = 0; i < len; ++i) {
        sum += labelList[i];
        // sum += *(_labelList[i]);
		// sum  += _block[i]->label;
		// sum  += block[i].label;
    }

	float beforeVar = -1 * sum / len * sum;

    int spIdx = 0;
    float _sum = 0.0;
    for (int i = 1; i < len; ++i) {
        if (spIdx > spLen) {
            break;
        }
        // _sum += block[i - 1].label;
        // _sum += _block[i - 1]->label;
        _sum += labelList[i - 1];
        // _sum += *(_labelList[i - 1]);
        if (feaList[i - 1] < spList[spIdx] && spList[spIdx] < feaList[i] ) {
        // if (_block[i - 1]->fea < spList[spIdx] && spList[spIdx] < _block[i]->fea ) {
        // if (block[i - 1].fea < spList[spIdx] && spList[spIdx] < block[i].fea ) {
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

	improve = beforeVar - minVar;
	// printf("improve:%lf\n", improve);
}



struct ParamFeaSort {
    Matrix* data;
    map<int, vector<int>*>* sortIdMap;
    vector<int>* itemVec;
    vector<float>* predVec;
    vector<int>* feaVec;
    int groupId;
    int step;
    vector<float*>* feaDataVec_;
    vector<float*>* labelDataVec_;
};

struct ParamFea {
    Matrix* data;
    map<int, vector<Block>*>* sortIdMap;
    vector<int>* itemVec;
    vector<float>* predVec;
    vector<int>* feaVec;
    int groupId;
    int step;
    int minFea;
    float minSp;
    float minVar;
};




void* threadFeaSort(void* param_ptr) {
    struct ParamFeaSort* param = (struct ParamFeaSort*)param_ptr;
    Matrix* data = param->data;
    vector<int>* feaVec = param->feaVec;
    vector<int>* itemVec = param->itemVec;
    vector<float>* predVec = param->predVec;

    int dataLen = param->data->length();
    int feaSize = 0;
    int itemSize = 0;
    for (int i = 0; i < feaVec->size(); ++i) { feaSize += feaVec->at(i); }
    for (int i = 0; i < itemVec->size(); ++i) { itemSize += itemVec->at(i); }

    // printf("item size:%d\n", itemSize);

    int start = param->groupId * param->step;
    int end = (param->groupId + 1) * param->step - 1;
    if (end > feaSize - 1) {
        end = feaSize - 1;
    }
    // printf("start:%d end:%d\n", start, end);

    int sum = 0;
    int left = 0;
    int right = 0;
    for (int i = 0; i < feaVec->size(); ++i) {
        sum += feaVec->at(i);
        if (sum == start + 1) { left = i; break;}
    }
    sum = 0;
    for (int i = 0; i < feaVec->size(); ++i) {
        sum += feaVec->at(i);
        if (sum == end + 1) { right = i; break;}
    }

    // printf("run 5.2\n");
    // printf("start:%d end:%d\n", start, end);
    // printf("left:%d right:%d\n", left, right);

    for (int i = left; i <= right; ++i) {
        int fea = i;
        if (feaVec->at(fea) < 1) {
            param->feaDataVec_->at(fea) = NULL;
            param->labelDataVec_->at(fea) = NULL;
            // printf("fea null:%d\n", fea);
            continue;
        }
        // printf("fea not null:%d\n", fea);
        // vector<int>* sortId = param->sortIdMap->at(fea);
        vector<int>* sortId = (*param->sortIdMap)[fea];
        float* feaDataVec = (float*)malloc(sizeof(float) * itemSize);
        float* labelDataVec = (float*)malloc(sizeof(float) * itemSize);

        double begin = getTime();
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            int id = sortId->at(j);
            for (int k = 0; k < itemVec->at(id); ++k) {
                feaDataVec[_idx] = data->at(id, fea);
                labelDataVec[_idx] = predVec->at(id);
                ++_idx;
            }
            if (_idx >= itemSize) { break; }
        }
        printf("fea:%lf size:%d idx:%d\n", getTime() - begin, _idx, fea);
        param->feaDataVec_->at(fea) = feaDataVec;
        param->labelDataVec_->at(fea) = labelDataVec;

    }
}

void* threadFea(void* param_ptr) {
    struct ParamFea* param = (struct ParamFea*)param_ptr;
    Matrix* data = param->data;
    vector<int>* feaVec = param->feaVec;
    vector<int>* itemVec = param->itemVec;
    vector<float>* predVec = param->predVec;

    int dataLen = param->data->length();
    int feaSize = 0;
    int itemSize = 0;
    for (int i = 0; i < feaVec->size(); ++i) { feaSize += feaVec->at(i); }
    for (int i = 0; i < itemVec->size(); ++i) { itemSize += itemVec->at(i); }

    // printf("item size:%d\n", itemSize);

    int start = param->groupId * param->step;
    int end = (param->groupId + 1) * param->step - 1;
    if (end > feaSize - 1) {
        end = feaSize - 1;
    }
    // printf("start:%d end:%d\n", start, end);

    int sum = 0;
    int left = 0;
    int right = 0;
    for (int i = 0; i < feaVec->size(); ++i) {
        sum += feaVec->at(i);
        if (sum == start + 1) { left = i; break;}
    }
    sum = 0;
    for (int i = 0; i < feaVec->size(); ++i) {
        sum += feaVec->at(i);
        if (sum == end + 1) { right = i; break;}
    }

    // printf("start:%d end:%d\n", start, end);
    // printf("left:%d right:%d\n", left, right);

    for (int i = left; i <= right; ++i) { 
        if (feaVec->at(i) < 1) {
            continue;
        }

        int fea = i;


        // vector<int>* sortId = (*sortIdMap)[fea];
        // vector<Block>* sortId = param->sortIdMap->at(fea);
        vector<Block>* sortId = (*param->sortIdMap)[fea];

        float feaDataVec[itemSize];
        float labelDataVec[itemSize];
        // float* feaDataVec = (float*)malloc(sizeof(float) * itemSize);
        // float* labelDataVec = (float*)malloc(sizeof(float) * itemSize);
        float* predVec_ = (float*)malloc(sizeof(float) * predVec->size());
        for (int _i = 0; _i < predVec->size(); ++_i) {
            predVec_[_i] = predVec->at(_i);
        }

        double pre_begin = getTime();
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            Block block = sortId->at(j);
            int _n = itemVec->at(block.id);
            if (_n <= 0) { continue; }
            for (int k = 0; k < _n; ++k) {
                feaDataVec[_idx] = block.fea;
                labelDataVec[_idx] = block.label;
                ++_idx;
            }
            if (_idx >= itemSize) { break; }
        }

        /*
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            int id = sortId->at(j);
            int _n = itemVec->at(id);
            if (_n <= 0) { continue; }
            for (int k = 0; k < _n; ++k) {
                feaDataVec[_idx] = data->at(id, fea);
                labelDataVec[_idx] = predVec->at(id);
                ++_idx;
            }
            if (_idx >= itemSize) { break; }
        }
        */
        printf("wise_pre_sort:%lf\n", getTime() - pre_begin);
        
        // printf("fea:%d pre_find:%lf\n", fea, getTime() - pre_begin);

        vector<float> spVec;
        for (int j = 1; j < itemSize; ++j) {
            if (feaDataVec[j - 1] + 1e-7 < feaDataVec[j]) {
                spVec.push_back(0.5 * (feaDataVec[j - 1] + feaDataVec[j]));
            }
        }

        if (spVec.size() <= 0) {
            // printf("fea:%d spVec is none\n", fea);
            continue;
        }

        float curSp = FLT_MAX;
        float curVar = FLT_MAX;

        double begin = getTime();
        gbdt_findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);
        printf("sp size:%d item:%d last:%lf\n", spVec.size(), itemSize, getTime() - begin);

        if (curVar < param->minVar) {
            param->minFea = fea;
            param->minSp = curSp;
            param->minVar = curVar;
        }
        // free(feaDataVec);
        // free(labelDataVec);
    }
}




// static void findBestSpV2Thread_(Matrix& data,
static void findBestSpV2Thread_(Matrix* data,
                          // map<int, vector<int> >* sortIdMap,
                          // map<int, vector<int>*>& sortIdMap,
                          map<int, vector<Block>*>* sortIdMap,
                          vector<int>& itemVec,
                          vector<int>& feaVec,
                          vector<float>& predVec,
                          int& minFea, float& minSp, float& minVar) {

    minFea = -1;
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    // printf("run 3.0\n");
    double begin = getTime();

    // int rangeSize = data.getRange();
    // int dataLen   = data.length();
    int rangeSize = data->getRange();
    int dataLen   = data->length();
    int itemSize  = 0;
    for (int i = 0; i < dataLen; ++i) {
        itemSize += itemVec[i];
    }

    int feaSize = 0;
    for (int i = 0; i < feaVec.size(); ++i) {
        feaSize += feaVec[i];
    }

    int threadSize = 4;
    int step = (feaSize + threadSize - 1) / threadSize;
    // printf("feaSize:%d step:%d\n", feaSize, step);

    struct ParamFea params[threadSize];
    for (int i = 0; i < threadSize; ++i) {
        // params[i].data = &data;
        params[i].data = data;
        params[i].sortIdMap = sortIdMap;
        params[i].itemVec = &itemVec;
        params[i].predVec = &predVec;
        params[i].feaVec = &feaVec;
        params[i].groupId = i;
        params[i].step = step;
        params[i].minFea = -1;
        params[i].minSp = FLT_MAX;
        params[i].minVar = FLT_MAX;
    }

    pthread_t threadPool[threadSize];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize (&attr, 16*1024*1024);    //修改栈大小为16M
    // printf("run 3.2\n");

    for (int i = 0; i < threadSize; ++i) {
        int ret = pthread_create(&threadPool[i], &attr, threadFea, (void*)&params[i]);
        if (ret) {
            printf("thread create error!\n");
        }
    }

    for (int i = 0; i < threadSize; ++i) {
        void* status;
        pthread_join(threadPool[i], &status);
    }

    // printf("node:%lf\n", getTime() - begin);

    for (int i = 0; i < threadSize; ++i) {
        if (params[i].minVar < minVar) {
            // printf("curVar:%f minVar:%f\n", curVar, minVar);
            minFea = params[i].minFea;
            minSp  = params[i].minSp;
            minVar = params[i].minVar;
        }
    }
 
}





static void findBestSpV2Thread(Matrix& data,
                          // map<int, vector<int> >* sortIdMap,
                          map<int, vector<int>*>& sortIdMap,
                          vector<int>& itemVec,
                          vector<int>& feaVec,
                          vector<float>& predVec,
                          int& minFea, float& minSp, float& minVar) {

    minFea = -1;
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    // printf("run 3.0\n");
    double begin = getTime();

    int rangeSize = data.getRange();
    int dataLen   = data.length();
    int itemSize  = 0;
    int feaSize = 0;
    for (int i = 0; i < dataLen; ++i) {
        itemSize += itemVec[i];
    }

    vector<float*> feaDataVec_;   feaDataVec_.resize(feaVec.size());
    vector<float*> labelDataVec_; labelDataVec_.resize(feaVec.size());
    for (int i = 0; i < feaVec.size(); ++i) {
        feaSize += feaVec[i];
        if (feaVec[i] > 0) {
            feaDataVec_[i] = (float*)malloc(sizeof(float) * itemSize);
            labelDataVec_[i] = (float*)malloc(sizeof(float) * itemSize);
        }
    }

    // printf("run 3.1\n");

    int threadSize = 4;
    int step = (feaSize + threadSize - 1) / threadSize;
    // printf("feaSize:%d step:%d\n", feaSize, step);

    struct ParamFeaSort params[threadSize];
    for (int i = 0; i < threadSize; ++i) {
        params[i].data = &data;
        params[i].sortIdMap = &sortIdMap;
        params[i].itemVec = &itemVec;
        params[i].predVec = &predVec;
        params[i].feaVec = &feaVec;
        params[i].groupId = i;
        params[i].step = step;
        params[i].feaDataVec_ = &feaDataVec_;
        params[i].labelDataVec_ = &labelDataVec_;
    }

    pthread_t threadPool[threadSize];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize (&attr, 16*1024*1024);    //修改栈大小为16M
    // printf("run 3.2\n");

    for (int i = 0; i < threadSize; ++i) {
        int ret = pthread_create(&threadPool[i], &attr, threadFeaSort, (void*)&params[i]);
        if (ret) {
            printf("thread create error!\n");
        }
    }

    for (int i = 0; i < threadSize; ++i) {
        void* status;
        pthread_join(threadPool[i], &status);
    }

    printf("sort:%lf\n", getTime() - begin);

    for (int i = 0; i < feaVec.size(); ++i) {

        if (feaVec[i] < 1) {
            continue;
        }

        double pre_begin = getTime();
        int fea = i;

        double feaBegin = getTime();

        /*
        // vector<int> sortId = (*sortIdMap)[fea];
        vector<int>* sortId = (*sortIdMap)[fea];

        float feaDataVec[itemSize];
        float labelDataVec[itemSize];
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            // int id = sortId[j];
            int id = sortId->at(j);
            if (itemVec[id] <= 0) {
                continue;
            }
            for (int k = 0; k < itemVec[id]; ++k) {
                feaDataVec[_idx] = data->at(id, fea);
                labelDataVec[_idx] = predVec->at(id);
                ++_idx;
            }
            if (_idx >= itemSize) {
                break;
            }
        }
        */
        float* feaDataVec = feaDataVec_[fea];
        float* labelDataVec = labelDataVec_[fea];
        printf("pre_find:%lf\n", getTime() - pre_begin);
        // printf("pre item size:%d\n", itemSize);
        // printf("feaDataVec addr:%ld fea:%d\n", feaDataVec, fea);

        vector<float> spVec;
        for (int j = 1; j < itemSize; ++j) {
            if (feaDataVec[j - 1] + 1e-7 < feaDataVec[j]) {
                spVec.push_back(0.5 * (feaDataVec[j - 1] + feaDataVec[j]));
            }
        }
        printf("pre_find:%lf\n", getTime() - pre_begin);

        if (spVec.size() <= 0) {
            // printf("fea:%d spVec is none\n", fea);
            continue;
        }

        float curSp = FLT_MAX;
        float curVar = FLT_MAX;

        double begin = getTime();
        gbdt_findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);
        printf("sp size:%lu item:%d last:%lf\n", spVec.size(), itemSize, getTime() - begin);

        if (curVar < minVar) {
            // printf("curVar:%f minVar:%f\n", curVar, minVar);
            minFea = fea;
            minSp  = curSp;
            minVar = curVar;
        }

    }
 
}




static void findBestSpV2_(Matrix* data,
                          // map<int, vector<int> >* sortIdMap,
                          // map<int, vector<int>*>* sortIdMap,
                          map<int, vector<Block>*>* sortIdMap,
                          vector<int>& itemVec,
                          vector<int>& feaVec,
                          vector<float>* predVec,
                          int& minFea, float& minSp, float& minVar) {
    /*
    minFea = -1;
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    int rangeSize = data->getRange();
    int dataLen   = data->length();
    int itemSize  = 0;

    for (int i = 0; i < dataLen; ++i) {
        itemSize += itemVec[i];
    }

    // printf("run 2.0.0.1\n");
    for (int i = 0; i < feaVec.size(); ++i) {

        if (feaVec[i] < 1) {
            continue;
        }
        int fea = i;

        // printf("run 1\n");
        // time_t feaBegin = time(NULL);
        double feaBegin = getTime();

        // printf("run 2.0.0.2\n");

        vector<int> sortId = (*sortIdMap)[fea];

        float feaDataVec[itemSize];
        float labelDataVec[itemSize];
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            int id = sortId[j];
            for (int k = 0; k < itemVec[id]; ++k) {
                feaDataVec[_idx] = data->at(id, fea);
                // labelDataVec[_idx] = data->at(id, rangeSize - 1);
                labelDataVec[_idx] = predVec->at(id);
                ++_idx;
            }
        }
        // printf("run 2.0.0.3\n");

        vector<float> spVec;
        for (int j = 1; j < itemSize; ++j) {
            if (feaDataVec[j - 1] + 1e-7 < feaDataVec[j]) {
                spVec.push_back(0.5 * (feaDataVec[j - 1] + feaDataVec[j]));
            }
        }

        // printf("run 2.0.0.4\n");

        if (spVec.size() <= 0) {
            // printf("fea:%d spVec is none\n", fea);
            continue;
        }

        float curSp = FLT_MAX;
        float curVar = FLT_MAX;

        gbdt_findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);

        if (curVar < minVar) {
            // printf("curVar:%f minVar:%f\n", curVar, minVar);
            minFea = fea;
            minSp  = curSp;
            minVar = curVar;
        }

        // printf("run 2.0.0.5\n");

        // printf("feaLoop 5:%lf\n", time(NULL) - feaBegin);
        // printf("data size:%d sp size:%ld\n", itemSize, spVec.size());
        // printf("curfea: %d %.3f %.3f\n", fea, curSp, curVar);
        //
        // printf("feaLoop time:%lf\n", time(NULL) - feaBegin);
    }
    */
    // printf("minfea: %d %.3f %.3f\n", minFea, minSp, minVar);

    minFea = -1;
    minSp = FLT_MAX;
    minVar = FLT_MAX;

    int rangeSize = data->getRange();
    int dataLen   = data->length();
    int itemSize  = 0;
    for (int i = 0; i < dataLen; ++i) {
        itemSize += itemVec[i];
    }

    for (int i = 0; i < feaVec.size(); ++i) {

        if (feaVec[i] < 1) {
            continue;
        }

        double pre_begin = getTime();
        int fea = i;

        double feaBegin = getTime();

        // vector<int> sortId = (*sortIdMap)[fea];
        vector<Block>* sortId = (*sortIdMap)[fea];
        printf("pre_find:%lf\n", getTime() - pre_begin);

        float feaDataVec[itemSize];
        float labelDataVec[itemSize];
        int _idx = 0;
        for (int j = 0; j < dataLen; ++j) {
            Block block = sortId->at(j);
            if (itemVec[block.id] <= 0) {
                continue;
            }
            for (int k = 0; k < itemVec[block.id]; ++k) {
                feaDataVec[_idx] = block.fea;
                labelDataVec[_idx] = block.label;
                ++_idx;
            }
            if (_idx >= itemSize) {
                break;
            }
 
            /*
            // int id = sortId[j];
            int id = sortId->at(j);
            if (itemVec[id] <= 0) {
                continue;
            }
            for (int k = 0; k < itemVec[id]; ++k) {
                feaDataVec[_idx] = data->at(id, fea);
                labelDataVec[_idx] = predVec->at(id);
                ++_idx;
            }
            if (_idx >= itemSize) {
                break;
            }
            */
        }
        printf("pre_find:%lf\n", getTime() - pre_begin);

        vector<float> spVec;
        for (int j = 1; j < itemSize; ++j) {
            if (feaDataVec[j - 1] + 1e-7 < feaDataVec[j]) {
                spVec.push_back(0.5 * (feaDataVec[j - 1] + feaDataVec[j]));
            }
        }
        printf("pre_find:%lf\n", getTime() - pre_begin);

        if (spVec.size() <= 0) {
            // printf("fea:%d spVec is none\n", fea);
            continue;
        }

        float curSp = FLT_MAX;
        float curVar = FLT_MAX;

        double begin = getTime();
        gbdt_findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);
        printf("sp size:%lu item:%d last:%lf\n", spVec.size(), itemSize, getTime() - begin);

        if (curVar < minVar) {
            // printf("curVar:%f minVar:%f\n", curVar, minVar);
            minFea = fea;
            minSp  = curSp;
            minVar = curVar;
        }

        // printf("feaLoop time:%lf\n", time(NULL) - feaBegin);
    }
 
}

void* threadNodeGroup(void* params_ptr) {
    struct Param* param = (struct Param*)params_ptr;

    int start = param->groupId * param->step;
    int end = (param->groupId + 1) * param->step - 1;
    if (end >= param->nodes->size()) {
        end = param->nodes->size() - 1;
    }
    vector<float>* predVec = param->predVec;
    param->newNodes = new vector<TreeNode*>();

    for (int _i = start; _i <= end; ++_i) {

        TreeNode* node = param->nodes->at(_i);

        // printf("run 2.0.1\n");
        
        // float lr = trainConf.shrink;

        // 节点覆盖的最小数据量
        int minCoverNum = trainConf.minCoverNum;
        if (node->size <= minCoverNum) {
            continue;
        }

        // 最大深度
        int maxDepth = trainConf.maxDepth;
        if (node->depth >= maxDepth - 1) {
            continue;
        }

        // 映射mask
        vector<int> itemVec;
        itemVec.resize(param->predVec->size());
        for (size_t i = 0; i < itemVec.size(); ++i) {
            itemVec[i] = 0;
        }
        for (int i = 0; i < node->size; ++i) {
            itemVec[node->itemIds[i]] += 1;
        }
        vector<int> feaVec;
        for (int i = 0; i < param->feaVec->size(); ++i) {
            feaVec.push_back(param->feaVec->at(i));
        }

        // printf("run 2.0.2\n");
        int minFea = -1;
        float minSp = FLT_MAX;
        float minVar = FLT_MAX;

        // printf("loop1:%ld\n", time(NULL) - begin);

        double begin = getTime();
        findBestSpV2_(param->data, param->sortIdMap, itemVec, feaVec, predVec, minFea, minSp, minVar);
        printf("node spend:%lf node.item:%d fea:%lu\n", getTime() - begin, node->size, feaVec.size());

        // printf("run 2.0.3\n");

        // 无分裂点
        if (minFea < 0) {
            continue;
        }

        node->fidx = minFea;
        node->split = minSp;

        vector<int> leftItems;
        leftItems.resize(itemVec.size());
        vector<int> rightItems;
        rightItems.resize(itemVec.size());
        int leftSize = 0;
        int rightSize = 0;

        for (int i = 0; i < itemVec.size(); ++i) {
            leftItems[i] = 0;
            rightItems[i] = 0;
            if (itemVec[i] < 1) {
                continue;
            }
            if (param->data->at(i, minFea) <= minSp) {
                leftItems[i] = itemVec[i];
                leftSize += 1;
            } else {
                rightItems[i] = itemVec[i];
                rightSize += 1;
            }
        }

        // 无法继续划分
        if (leftSize <= 0 || rightSize <= 0) {
            printf("左子树或右子树为空: %d\t%f\t%f\n", minFea, minSp, minVar);
        }

        // printf("run 2.0.4\n");

        TreeNode* leftNode  = constructTreeNode(leftItems, param->data, predVec);
        TreeNode* rightNode = constructTreeNode(rightItems, param->data, predVec);

        leftNode->depth = node->depth + 1;
        rightNode->depth = node->depth + 1;

        node->left = leftNode;
        node->right = rightNode;

        param->newNodes->push_back(leftNode);
        param->newNodes->push_back(rightNode);
    }

    return (void*)0;
}






void* threadNode(void* params_ptr) {
    struct Param* param = (struct Param*)params_ptr;

    TreeNode* node = param->nodes->at(param->idx);

    vector<float>* predVec = param->predVec;

    param->newNodes = new vector<TreeNode*>();
    // printf("run 2.0.1\n");
    
    // float lr = trainConf.shrink;

    // 节点覆盖的最小数据量
    int minCoverNum = trainConf.minCoverNum;
    if (node->size <= minCoverNum) {
        return (void*)0;
    }

    // 最大深度
    int maxDepth = trainConf.maxDepth;
    if (node->depth >= maxDepth - 1) {
        return (void*)0;
    }

    vector<int> itemVec;
    itemVec.resize(param->predVec->size());
    for (size_t i = 0; i < itemVec.size(); ++i) {
        itemVec[i] = 0;
    }
    for (int i = 0; i < node->size; ++i) {
        itemVec[node->itemIds[i]] += 1;
    }
    vector<int> feaVec;
    for (int i = 0; i < param->feaVec->size(); ++i) {
        feaVec.push_back(param->feaVec->at(i));
    }

    // printf("run 2.0.2\n");
    int minFea = -1;
    float minSp = FLT_MAX;
    float minVar = FLT_MAX;

    // printf("loop1:%ld\n", time(NULL) - begin);

    findBestSpV2_(param->data, param->sortIdMap, itemVec, feaVec, predVec, minFea, minSp, minVar);

    // printf("run 2.0.3\n");

    // 无分裂点
    if (minFea < 0) {
        return (void*)0;
    }

    node->fidx = minFea;
    node->split = minSp;

    vector<int> leftItems;
    leftItems.resize(itemVec.size());
    vector<int> rightItems;
    rightItems.resize(itemVec.size());
    int leftSize = 0;
    int rightSize = 0;

    for (int i = 0; i < itemVec.size(); ++i) {
        leftItems[i] = 0;
        rightItems[i] = 0;
        if (itemVec[i] < 1) {
            continue;
        }
        if (param->data->at(i, minFea) <= minSp) {
            leftItems[i] = itemVec[i];
            leftSize += 1;
        } else {
            rightItems[i] = itemVec[i];
            rightSize += 1;
        }
    }

    // 无法继续划分
    if (leftSize <= 0 || rightSize <= 0) {
        printf("左子树或右子树为空: %d\t%f\t%f\n", minFea, minSp, minVar);
    }

    // printf("run 2.0.4\n");

    TreeNode* leftNode  = constructTreeNode(leftItems, param->data, predVec);
    TreeNode* rightNode = constructTreeNode(rightItems, param->data, predVec);

    leftNode->depth = node->depth + 1;
    rightNode->depth = node->depth + 1;

    node->left = leftNode;
    node->right = rightNode;

    param->newNodes->push_back(leftNode);
    param->newNodes->push_back(rightNode);

    return (void*)0;
}




/*
int _comp(const pair<int, float>& a, const pair<int, float>& b) {
    return a.second < b.second;
}
*/

void* threadFeaGroupV2(void* param_ptr) {

	double func_begin = getTime();

    struct  ParamLevel* param = (struct ParamLevel*)param_ptr;
    Matrix* data = param->data;
    map<int, vector<Block>*>* sortIdMap = param->sortIdMap;
    map<int, Block*>* p_sortIdMap = param->p_sortIdMap;
	map<int, int*>*   p_sort2IdMap = param->p_sort2IdMap;
    vector<int>*   feaVec  = param->feaVec;
    vector<float>* predVec = param->predVec;

    int dataLen = data->length();
    int feaSize = 0;
    for (size_t i = 0; i < feaVec->size(); ++i) { feaSize += feaVec->at(i); }

	/*
    int start = param->groupId * param->step;
    int end = (param->groupId + 1) * param->step - 1;
    end = end > feaSize - 1 ? feaSize - 1 : end;
	*/
    int start = param->start;
    int end   = param->end;
    printf("start:%d end:%d\n", start, end);

	if (start > end) {
		printf("start end error!\n");
		return (void*)0;
	}

    int left = -1;
    int right = -1;
    for (int i = 0, sum = -1; i < static_cast<int>(feaVec->size()); ++i) {
        sum += feaVec->at(i);
        if (sum == start) { left  = i; break; }
    }
    for (int i = 0, sum = -1; i < static_cast<int>(feaVec->size()); ++i) {
        sum += feaVec->at(i);
        if (sum == end )   { right = i; break; }
    }
    printf("left:%d right:%d\n", left, right);

	int maxSize = 0;
    for (size_t n = 0; n < param->nodes->size(); ++n) {
		maxSize = std::max(maxSize, param->nodes->at(n)->size);
	}
	float** _labelDataVec = param->_labelDataVec;

	// float* feaDataVec = param->feaDataVec;
	// float* labelDataVec = param->labelDataVec;
	// float* spVec = param->spVec;
	// maxSize = dataLen;
	float feaDataVec[maxSize];
	float labelDataVec[maxSize];
	float spVec[maxSize];

	Block itBlock[maxSize];
	Block* _itBlock[maxSize];


	// vector<int> itemVec(predVec->size());
	// int* itemVec = param->itemMask;
	// int8_t* itemVec = param->itemMask;
	int8_t itemVec[dataLen];
	int _itemVec[dataLen];
	
	printf("pref_%d:%lf\n", param->groupId, getTime() - func_begin);

	int8_t tmp_sort[dataLen];
	int tmp_sort2Id[dataLen];
	for (int i = 0; i < dataLen; ++i) {
		tmp_sort2Id[i] = dataLen - i - 1;
	}


	double for_begin = getTime();
	int8_t zero = 0;
    for (size_t n = 0; n < param->nodes->size(); ++n) {
        TreeNode* node = param->nodes->at(n);
        // 节点覆盖的最小数据量
        if (node->size <= trainConf.minCoverNum) { continue; }

        // 最大深度
        if (node->depth >= trainConf.maxDepth - 1) { continue; }

		double set_begin = getTime();
        // 映射mask
        size_t itemSize = node->size;
        // vector<int> itemVec(predVec->size());
        // for (size_t i = 0; i < itemVec.size(); ++i) { itemVec[i] = 0; }
		/*
        for (size_t i = 0; i < predVec->size(); ++i) { itemVec[i] = 0; }
        for (size_t i = 0; i < itemSize; ++i) { itemVec[node->itemIds[i]] += 1; }
		*/
		// int* itemVec_p = itemVec;
		memset(itemVec, 0, 1 * dataLen);
		/* int8_t* _p = itemVec;
		int8_t* _q = itemVec + dataLen;
		while (_p < _q) {
			*_p++ = 0;
		} */
		// memset(itemVec, 0, 4 * (dataLen + 31) / 32);
		// memset(_itemVec, 0, 4 * dataLen);
		// memset(itemVec, 0, sizeof(int) * dataLen);
		// int* _ids_p = node->itemIds;
        // for (size_t i = 0; i < itemSize; ++i) { itemVec[*_ids_p++] += 1; }
		
		int _id = 0;
        for (size_t i = 0; i < itemSize; ++i) {
			itemVec[node->itemIds[i]] += 1;
			// _itemVec[node->itemIds[i]] += 1;
			// _id = node->itemIds[i];
			// itemVec[_id >> 5] |= 1 << (_id & 0x1F); 
		}
		printf("set_%d:%lf\n", param->groupId, getTime() - set_begin);

		Block* block_p;
		Block* block_p_end;
		float* feaDataVec_p;
		float* labelDataVec_p;
		Block* itBlock_p;
        
		// double fea_begin = getTime();
		// int tmp_id = 0;
        // for (int _i = start; _i <= end; ++_i) {
        for (int _i = left; _i <= right; ++_i) {

            if (feaVec->at(_i) < 1) { 
				continue;
			}
            // double pre_begin = getTime();
			// printf("fealoop_%d:%lf\n", param->groupId, getTime() - pre_begin);

            int fea = _i;

			/*
			int tmp = 0;
            block_p = (*p_sortIdMap)[fea];
            block_p_end = block_p + dataLen;
			for (; block_p < block_p_end; ++block_p) {
                if (itemVec[block_p->id] <= 0) { continue; }
				++tmp;
                if (tmp >= itemSize) { break; }
			}
			*/
			
			/*
			int* itemVec_p = _itemVec;
			int _tmp = 0;
			for (int j = 0; j < dataLen; ++j) {
				if (_itemVec[j] <= 0) {
					_tmp += 2;
				}
			}
			*/

			int* tmp_sort2Id = (*p_sort2IdMap)[fea];

			memset(tmp_sort, 0, dataLen);
			for (size_t k = 0; k < itemSize; ++k) {
				int idx = tmp_sort2Id[node->itemIds[k]];
				tmp_sort[idx] = 1;
			}

            int _idx = 0;
            block_p = (*p_sortIdMap)[fea];
			itBlock_p = itBlock;
			for (size_t k = 0; k < dataLen; ++k) {
				if (tmp_sort[k] == 0) { continue; }
				*itBlock_p++ = *(block_p + k);
				++_idx;
                if (_idx >= itemSize) { break; }
			}

            
            /*Block* block_p = (*p_sortIdMap)[fea];
            Block* block_p_end = block_p + dataLen - 1;
			float* feaDataVec_p   = feaDataVec;
			float* labelDataVec_p = labelDataVec; */
			/*
            int _idx = 0;
            block_p = (*p_sortIdMap)[fea];
            block_p_end = block_p + dataLen;
			feaDataVec_p = feaDataVec;
			labelDataVec_p = labelDataVec;
			itBlock_p = itBlock;
			Block** _itBlock_p = _itBlock;
			// for (int j = 0; j < dataLen; ++j, ++block_p) {
			// int a = 10;
			for (; block_p < block_p_end; ++block_p) {
                if (itemVec[block_p->id] <= 0) { continue; }
				// tmp_id = block_p->id;
                // if ((itemVec[tmp_id >> 5] & (1 << (tmp_id & 0x1F))) == 0) { continue; }
                // for (int k = 0; k < itemVec[block_p->id]; ++k) {
                    // *feaDataVec_p++   = block_p->fea;
                    // *labelDataVec_p++ = block_p->label;
					// itBlock_p->fea = block_p->fea;
					// itBlock_p->label = block_p->label;
					*itBlock_p++ = *block_p;
					// *_itBlock_p++ = block_p;
                    ++_idx;
                // }
                if (_idx >= itemSize) { break; }
			}
			*/

			/*
            int _idx = 0;
            // vector<Block>* sortId = sortIdMap->at(fea);
            vector<Block>* sortId = (*sortIdMap)[fea];
			// Block* sortId = (*p_sortIdMap)[fea];
            for (int j = 0; j < dataLen; ++j) {
                // Block& block = sortId->at(j);
                Block* block = &sortId->at(j);
				// Block* block = sortId + j;
                if (itemVec[block->id] <= 0) { continue; }
                for (int k = 0; k < itemVec[block->id]; ++k) {
                    feaDataVec[_idx]   = block->fea;
                    labelDataVec[_idx] = block->label;
                    // labelDataVec[_idx]  = predVec->at(block.id);
                    // labelDataVec[_idx]  = *(block._label);
                    // _labelDataVec[_idx] = block._label;
                    ++_idx;
                }
                if (_idx >= itemSize) { break; }
            }
			*/

            // printf("pre-find:%lf\n", getTime() - pre_begin);
            
            // vector<float> spVec;
			// spVec.reserve(itemSize);
			int spIdx = 0;
			float last_value = 0.0f;
			feaDataVec_p = feaDataVec + 1;
		   	last_value = feaDataVec[0];
		   	last_value = itBlock[0].fea;
            for (int j = 1; j < itemSize; ++j) {
                if (last_value + 1e-7 < itBlock[j].fea) {
                    spVec[spIdx] = 0.5 * (last_value + itBlock[j].fea);
					++spIdx;
					last_value = itBlock[j].fea;
                }
				/*
                if (itBlock[j - 1].fea + 1e-7 < itBlock[j].fea) {
                    spVec[spIdx] = 0.5 * (itBlock[j - 1].fea + itBlock[j].fea);
					++spIdx;
				}
				*/
				/*
				if (feaDataVec[j - 1] + 1e-7 < feaDataVec[j]) {
                    // spVec.push_back(0.5 * (feaDataVec[j - 1] + feaDataVec[j]));
                    spVec[spIdx] = 0.5 * (feaDataVec[j - 1] + feaDataVec[j]);
					++spIdx;
                }
			    */
				/*
				if (last_value + 1e-7 < *feaDataVec_p) {
					spVec[spIdx++] = 0.5 * (last_value + *feaDataVec_p);
					last_value = *feaDataVec_p;
				}
				*/
				++feaDataVec_p;
            }
            // printf("pre_find:%lf item_size:%d\n", getTime() - pre_begin, itemSize);

            // if (spVec.size() <= 0) {
            if (spIdx <= 0) {
                // printf("fea:%d spVec is none\n", fea);
				// printf("fealoop_%d:%lf\n", param->groupId, getTime() - pre_begin);
                continue;
            }

            // double sp_begin = getTime();
            float curSp = FLT_MAX;
            float curVar = FLT_MAX;
			float curImprove = 0.0f;

            // gbdt_findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);
            // printf("sp size:%lu item:%lu last:%lf\n", spVec.size(), itemSize, getTime() - begin);
            // gbdt_findLeastVarV2(spVec, spIdx, feaDataVec, labelDataVec, itemSize, curSp, curVar);
            // gbdt_findLeastVarV2(spVec, spIdx, feaDataVec, labelDataVec, _labelDataVec, itBlock, _itBlock, itemSize, curSp, curVar, curImprove);
            gbdt_findLeastVarV3(spVec, spIdx, feaDataVec, labelDataVec, _labelDataVec, itBlock, _itBlock, itemSize, curSp, curVar, curImprove);
            // printf("sp:%lf\n", getTime() - sp_begin);

			// param->staInfo->feaImprove[fea] += curImprove;
			// param->staInfo->feaSelect[fea] += 1;

			// 线程同步
			pthread_mutex_lock(&mutex);
            if (curVar < param->minVar->at(n)) {
                param->minFea->at(n) = fea;
                param->minSp->at(n)  = curSp;
                param->minVar->at(n) = curVar;
                param->improve->at(n) = curImprove;
                // printf("curVar:%f minVar:%f\n", curVar, minVar);
            }
			pthread_mutex_unlock(&mutex);
            // printf("sp size:%d item:%lu last:%lf\n", spIdx, itemSize, getTime() - begin);

        }
		// printf("feasloop_%d:%lf\n", param->groupId, getTime() - fea_begin);
    }

	printf("func_%d:%lf\n", param->groupId, getTime() - func_begin);

    return static_cast<void*>(0);
}

void* threadFeaGroup(void* params_ptr) {
    struct ParamLevel* param = (struct ParamLevel*)params_ptr;

    Matrix* data = param->data;
    map<int, vector<Block>*>* sortIdMap = param->sortIdMap;
    // vector<int>* itemVec   = param->itemVec;
    // vector<int>* feaVec    = param->feaVec;
    // vector<float>* predVec = param->predVec;

    int dataLen = data->length();
    int feaSize = 0;
    for (int i = 0; i < param->feaVec->size(); ++i) { feaSize += param->feaVec->at(i); }

    int start = param->groupId * param->step;
    int end = (param->groupId + 1) * param->step - 1;
    end = end > feaSize - 1 ? feaSize - 1 : end;

    printf("start:%d end:%d\n", start, end);

    int sum = 0;
    int left = 0;
    int right = 0;
    for (int i = 0; i < param->feaVec->size(); ++i) {
        sum += param->feaVec->at(i);
        if (sum == start + 1) { left = i; break;}
    }
    sum = 0;
    for (int i = 0; i < param->feaVec->size(); ++i) {
        sum += param->feaVec->at(i);
        if (sum == end + 1) { right = i; break;}
    }

    printf("left:%d right:%d\n", left, right);


    for (int _i = start; _i <= end; ++_i) {
        int fea = _i;
        if (param->feaVec->at(fea) < 1) { continue; }

        for (int i = 0; i < param->nodes->size(); ++i) {
            TreeNode* node = param->nodes->at(i);
            // 节点覆盖的最小数据量
            if (node->size <= trainConf.minCoverNum) { continue; }

            // 最大深度
            if (node->depth >= trainConf.maxDepth - 1) { continue; }

            // 映射mask
            int itemSize = node->size;
            vector<int> itemVec;
            itemVec.resize(param->predVec->size());
            for (size_t i = 0; i < itemVec.size(); ++i) { itemVec[i] = 0; }
            for (int i = 0; i < node->size; ++i) { itemVec[node->itemIds[i]] += 1; }

            // printf("item_size:%d\n", itemSize);

            vector<int> feaVec;
            for (int j = 0; j < param->feaVec->size(); ++j) { feaVec.push_back(param->feaVec->at(j)); }

            vector<Block>* sortId = (*sortIdMap)[fea];
            // printf("pre_find:%lf\n", getTime() - pre_begin);

            double pre_begin = getTime();
            float feaDataVec[itemSize];
            float labelDataVec[itemSize];
            int _idx = 0;
            for (int j = 0; j < dataLen; ++j) {
                Block block = sortId->at(j);
                if (itemVec[block.id] <= 0) { continue; }
                for (int k = 0; k < itemVec[block.id]; ++k) {
                    feaDataVec[_idx] = block.fea;
                    labelDataVec[_idx] = block.label;
                    ++_idx;
                }
                if (_idx >= itemSize) { break; }
            }
            printf("pre_find:%lf\n", getTime() - pre_begin);

            vector<float> spVec;
            for (int j = 1; j < itemSize; ++j) {
                if (feaDataVec[j - 1] + 1e-7 < feaDataVec[j]) {
                    spVec.push_back(0.5 * (feaDataVec[j - 1] + feaDataVec[j]));
                }
            }
            printf("pre_find:%lf\n", getTime() - pre_begin);

            if (spVec.size() <= 0) {
                // printf("fea:%d spVec is none\n", fea);
                continue;
            }

            float curSp = FLT_MAX;
            float curVar = FLT_MAX;

            double begin = getTime();
            gbdt_findLeastVarV1(spVec, feaDataVec, labelDataVec, itemSize, curSp, curVar);
            printf("sp size:%lu item:%d last:%lf\n", spVec.size(), itemSize, getTime() - begin);

            if (curVar < param->minVar->at(i)) {
                param->minFea->at(i) = fea;
                param->minSp->at(i)  = curSp;
                param->minVar->at(i) = curVar;
                // printf("curVar:%f minVar:%f\n", curVar, minVar);
            }
        }
    } 
    return (void*)0;
}


void levelWiseV3(Matrix* data,
                 map<int, vector<Block>*>* sortIdMap,
                 map<int, Block*>* p_sortIdMap,
				 map<int, int*>* p_sort2IdMap,
				 StaInfo* staInfo,
                 vector<float>& predVec,
                 vector<int>& itemVec,
                 vector<int>& feaVec,
				 vector<float*>& feaDataVec,
				 vector<float*>& labelDataVec,
				 // vector<float**>& _labelDataVec,
				 vector<float*>& spVec,
				 vector<int8_t*>&   itemMask,
				 vector<int>*   minFea,
				 vector<float>* minSp,
				 vector<float>* minVar,
				 vector<float>* improve,
                 int threadSize,
                 int depth,
                 vector<TreeNode*>& levelList,
                 vector<TreeNode*>& newLevelList) {

    int feaSize = 0;

	vector<int>* feaVec_p = new vector<int>(feaVec.size());
	vector<float>* predVec_p = new vector<float>(predVec.size());
	vector<TreeNode*>* levelList_p = new vector<TreeNode*>(levelList.size());

    for (size_t i = 0; i < feaVec.size(); ++i) {
		feaSize += feaVec[i];
		feaVec_p->at(i) = feaVec[i];
	}
    for (size_t i = 0; i < predVec.size(); ++i) { predVec_p->at(i) = predVec[i]; }
	for (size_t i = 0; i < levelList.size(); ++i) { levelList_p->at(i) = levelList[i]; }
	
	int x = 0;
	int y = 0;
	int step = 0;
	for (int a = 0; a <= threadSize; ++a) {
		int _t = (feaSize + a) % threadSize;
		if (_t == 0) {
			step = (feaSize + a) / threadSize;	
			x = a;
			y = threadSize - x;
			break;
		}
	}

	if (step <= 0) {
		printf("get step error!\n");
	}
    int _step = (feaSize + threadSize - 1) / threadSize;

	double param_begin = getTime();
    size_t nodeSize = levelList.size();
	for (size_t i = 0; i < nodeSize; ++i) {
		minFea->at(i) = -1;
		minSp->at(i)  = FLT_MAX;
		minVar->at(i) = FLT_MAX;
	}

	float* feaImprove = new float[feaVec.size()];
	memset(feaImprove, 0, feaVec.size() * sizeof(float));

    struct ParamLevel params[threadSize];
	int _idx = 0;
    for (size_t i = 0; i < threadSize; ++i) {
        params[i].data = data;
        params[i].sortIdMap = sortIdMap;
        params[i].p_sortIdMap = p_sortIdMap;
        params[i].p_sort2IdMap = p_sort2IdMap;
        // params[i].itemVec = &itemVec;
        // params[i].feaVec = &feaVec;
        // params[i].predVec = &predVec;
        params[i].feaVec = feaVec_p;
        params[i].predVec = predVec_p;
        params[i].groupId = i;
        params[i].step = _step;
        // params[i].nodes  = &levelList;
        params[i].nodes  = levelList_p;
        params[i].minFea = minFea;
        params[i].minSp  = minSp;
        params[i].minVar = minVar;

		params[i].feaDataVec = feaDataVec[i];
		params[i].labelDataVec = labelDataVec[i];
		// params[i]._labelDataVec = _labelDataVec[i];
		params[i].spVec = spVec[i];
		params[i].itemMask = itemMask[i];

		params[i].staInfo = staInfo;
		params[i].improve = improve;

		if (i + 1 <= x) {
			params[i].start = _idx;
			params[i].end = _idx + step - 1 - 1;
			_idx += step - 1;
		} else {
			params[i].start = _idx;
			params[i].end = _idx + step - 1;
			_idx += step;
		}
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 5*1024*1024);     // 线程栈50M

	printf("param:%lf\n", getTime() - param_begin);

	double thread_begin = getTime();
    pthread_t threadPool[threadSize];
    for (size_t i = 0; i < threadSize; ++i) {
        int ret = pthread_create(&threadPool[i], &attr, threadFeaGroupV2, (void*)&params[i]);
        if (ret) {
            printf("thread create error!\n");
        }
    }
    for (size_t i = 0; i < threadSize; ++i) {
        void* status;
        pthread_join(threadPool[i], &status);
    }
	printf("thread:%lf\n", getTime() - thread_begin);

	double begin = getTime();
    for (size_t i = 0; i < levelList.size(); ++i) {

        int _minFea = -1;
        float _minSp = FLT_MAX;
        float _minVar = FLT_MAX;
		float _improve = FLT_MAX;

        // for (int j = 0; j < threadSize; ++j) {
            if (minVar->at(i) < _minVar) {
                // minVar =yparams[j].minVar->at(i);
                // minFea = params[j].minFea->at(i);
                // minSp  = params[j].minSp->at(i);
				// delta = param[j].improve->at(i);
                _minVar = minVar->at(i);
                _minFea = minFea->at(i);
                _minSp  = minSp->at(i);
				_improve = improve->at(i);
            }
        // }

        if (_minFea < 0) { continue; }

		staInfo->feaImprove[_minFea] += _improve;
		staInfo->feaSelect[_minFea] += 1;

        TreeNode* node = levelList[i];

        node->fidx = _minFea;
        node->split = _minSp;

        vector<int> leftItems;
        leftItems.reserve(node->size);
        vector<int> rightItems;
        rightItems.reserve(node->size);

        for (size_t i = 0; i < node->size; ++i) {
            int id = node->itemIds[i];
            if (data->at(id, _minFea) <= _minSp) {
                leftItems.push_back(id);
            } else {
                rightItems.push_back(id);
            }
        }

        // 无法继续划分
        if (leftItems.size() <= 0 || rightItems.size() <= 0) {
            printf("左子树或右子树为空: %d\t%f\t%f\n", _minFea, _minSp, minVar);
        }

        node->left  = constructTreeNodeV1(leftItems, data, &predVec);
        node->right = constructTreeNodeV1(rightItems, data, &predVec);
        node->left->depth  = node->depth + 1;
        node->right->depth = node->depth + 1;
        
        newLevelList.push_back(node->left);
        newLevelList.push_back(node->right);
    }

	delete feaVec_p;
	delete predVec_p;
	delete levelList_p;

	printf("depth:%d gather:%lf node_size:%ld\n", depth, getTime() - begin, levelList.size());
}

void levelWiseV2(Matrix* data,
                 map<int, vector<Block>*>* sortIdMap,
                 vector<float>& predVec,
                 vector<int>& itemVec,
                 vector<int>& feaVec,
                 int threadSize,
                 int depth,
                 vector<TreeNode*>& levelList,
                 vector<TreeNode*>& newLevelList) {

    int feaSize = 0;
    for (int i = 0; i < feaVec.size(); ++i) { feaSize += feaVec[i]; }

    int step = (feaSize + threadSize - 1) / threadSize;


    int nodeSize = levelList.size();
    vector<int>* minFea   = new vector<int>(nodeSize, -1);
    vector<float>* minSp  = new vector<float>(nodeSize, FLT_MAX);
    vector<float>* minVar = new vector<float>(nodeSize, FLT_MAX);
    struct ParamLevel params[threadSize];
    for (int i = 0; i < threadSize; ++i) {
        params[i].data = data;
        params[i].sortIdMap = sortIdMap;
        params[i].itemVec = &itemVec;
        params[i].predVec = &predVec;
        params[i].feaVec = &feaVec;
        params[i].groupId = i;
        params[i].step = step;
        params[i].nodes  = &levelList;
        params[i].minFea = minFea;
        params[i].minSp  = minSp;
        params[i].minVar = minVar;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize (&attr, 50*1024*1024);    //修改栈大小为16M

    pthread_t threadPool[threadSize];
    for (int i = 0; i < threadSize; ++i) {
        int ret = pthread_create(&threadPool[i], &attr, threadFeaGroup, (void*)&params[i]);
        if (ret) {
            printf("thread create error!\n");
        }
    }
    for (int i = 0; i < threadSize; ++i) {
        pthread_join(threadPool[i], NULL);
    }

    for (int i = 0; i < levelList.size(); ++i) {
        TreeNode* node = levelList[i];
        int minFea = -1;
        float minSp = FLT_MAX;
        float minVar = FLT_MAX;
        for (int j = 0; j < threadSize; ++j) {
            if (params[j].minVar->at(i) < minVar) {
                minFea = params[j].minFea->at(i);
                minSp  = params[j].minSp->at(i);
                minVar = params[j].minVar->at(i);
            }
        }

        if (minFea < 0) { continue; }

        node->fidx = minFea;
        node->split = minSp;

        vector<int> leftItems(itemVec.size());
        vector<int> rightItems(itemVec.size());
        int leftSize = 0;
        int rightSize = 0;


        vector<int> _itemVec;
        _itemVec.resize(predVec.size());
        for (size_t i = 0; i < _itemVec.size(); ++i) { _itemVec[i] = 0; }
        for (int i = 0; i < node->size; ++i) { _itemVec[node->itemIds[i]] += 1; }

        for (int i = 0; i < _itemVec.size(); ++i) {
            leftItems[i] = 0;
            rightItems[i] = 0;
            if (_itemVec[i] < 1) { continue; }
            if (data->at(i, minFea) <= minSp) {
                leftItems[i] = _itemVec[i];
                leftSize += _itemVec[i];
            } else {
                rightItems[i] = _itemVec[i];
                rightSize += _itemVec[i];
            }
        }

        // 无法继续划分
        if (leftSize <= 0 || rightSize <= 0) {
            printf("左子树或右子树为空: %d\t%f\t%f\n", minFea, minSp, minVar);
        }

        TreeNode* leftNode  = constructTreeNode(leftItems, data, &predVec);
        TreeNode* rightNode = constructTreeNode(rightItems, data, &predVec);

        leftNode->depth = node->depth + 1;
        rightNode->depth = node->depth + 1;

        node->left = leftNode;
        node->right = rightNode;

        newLevelList.push_back(leftNode);
        newLevelList.push_back(rightNode);
    }
}




// void levelWise(Matrix& data, map<int, vector<int>*>& sortIdMap,
void levelWise(Matrix* data,
               map<int, vector<Block>*>* sortIdMap,
               vector<float>& predVec, vector<int>& itemVec, vector<int>& feaVec,
               int threadSize, int depth,
               vector<TreeNode*>& levelList, vector<TreeNode*>& newLevelList) {
    newLevelList.clear(); 
    for (int _i = 0; _i < levelList.size(); ++_i) {
        TreeNode* node = levelList.at(_i);

        // 节点覆盖的最小数据量
        int minCoverNum = trainConf.minCoverNum;
        if (node->size <= minCoverNum) {
            continue;
        }

        // 最大深度
        int maxDepth = trainConf.maxDepth;
        if (node->depth >= maxDepth - 1) {
            continue;
        }

        /*
        // 映射mask
        vector<int> itemVec;
        itemVec.resize(predVec.size());
        for (size_t i = 0; i < itemVec.size(); ++i) {
            itemVec[i] = 0;
        }
        for (int i = 0; i < node->size; ++i) {
            itemVec[node->itemIds[i]] += 1;
        }
        vector<int> feaVec;
        for (int i = 0; i < feaVec.size(); ++i) {
            feaVec.push_back(feaVec.at(i));
        }
        */

        // printf("run 2.0.1\n");
        int minFea = -1;
        float minSp = FLT_MAX;
        float minVar = FLT_MAX;

        // printf("loop1:%ld\n", time(NULL) - begin);
        // printf("run 2.0.2\n");

        double begin = getTime();
        // findBestSpV2_(param->data, param->sortIdMap, itemVec, feaVec, predVec, minFea, minSp, minVar);
        // findBestSpV2Thread(data, sortIdMap, itemVec, feaVec, predVec, minFea, minSp, minVar);
        findBestSpV2Thread_(data, sortIdMap, itemVec, feaVec, predVec, minFea, minSp, minVar);
        printf("node spend:%lf node.item:%d fea:%lu\n", getTime() - begin, node->size, feaVec.size());

        // printf("run 2.0.3\n");

        // 无分裂点
        if (minFea < 0) {
            continue;
        }

        node->fidx = minFea;
        node->split = minSp;

        vector<int> leftItems;
        leftItems.resize(itemVec.size());
        vector<int> rightItems;
        rightItems.resize(itemVec.size());
        int leftSize = 0;
        int rightSize = 0;

        for (int i = 0; i < itemVec.size(); ++i) {
            leftItems[i] = 0;
            rightItems[i] = 0;
            if (itemVec[i] < 1) {
                continue;
            }
            if (data->at(i, minFea) <= minSp) {
                leftItems[i] = itemVec[i];
                leftSize += 1;
            } else {
                rightItems[i] = itemVec[i];
                rightSize += 1;
            }
        }

        // 无法继续划分
        if (leftSize <= 0 || rightSize <= 0) {
            printf("左子树或右子树为空: %d\t%f\t%f\n", minFea, minSp, minVar);
        }

        // printf("run 2.0.4\n");

        TreeNode* leftNode  = constructTreeNode(leftItems, data, &predVec);
        TreeNode* rightNode = constructTreeNode(rightItems, data, &predVec);

        leftNode->depth = node->depth + 1;
        rightNode->depth = node->depth + 1;

        node->left = leftNode;
        node->right = rightNode;

        newLevelList.push_back(leftNode);
        newLevelList.push_back(rightNode);
    }
}
 
// void levelThread(Matrix& data, map<int, vector<int>*>& sortIdMap,
void levelThread(Matrix* data,
                 map<int, vector<Block>*>* sortIdMap,
                      vector<float>& predVec, vector<int>& itemVec, vector<int>& feaVec,
                      int threadSize, int depth, vector<TreeNode*>& levelList, vector<TreeNode*>& newLevelList) {

    int step = (levelList.size() + threadSize - 1) / threadSize;

    struct Param params[threadSize];
    for (int i = 0; i < threadSize; ++i) {
        // params[i].data = &data;
        params[i].data = data;
        params[i].sortIdMap = sortIdMap;
        params[i].depth = depth;
        params[i].groupId = i;
        params[i].step = step;
        params[i].nodes = &levelList;
        params[i].newNodes = NULL;
        params[i].feaVec = &feaVec;
        params[i].predVec = &predVec;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize (&attr, 50*1024*1024);    //修改栈大小为16M

    pthread_t threadPool[threadSize];
    for (int i = 0; i < threadSize; ++i) {
        int ret = pthread_create(&threadPool[i], &attr, threadNodeGroup, (void*)&params[i]);
        if (ret) {
            printf("thread create error!\n");
        }
    }
    for (int i = 0; i < threadSize; ++i) {
        pthread_join(threadPool[i], NULL);
    }

    newLevelList.clear();
    for (int i = 0; i < threadSize; ++i) {
        vector<TreeNode*>* _nodes = params[i].newNodes;
        for (int j = 0; j < _nodes->size(); ++j) {
            newLevelList.push_back(_nodes->at(j));
        }
    }
}


// TreeNode* gbdt_main(Matrix& data, map<int, vector<int>*>& sortIdMap,
TreeNode* gbdt_main(Matrix* data,
                    map<int, vector<Block>*>* sortIdMap,
                    map<int, Block*>* p_sortIdMap,
					map<int, int*>* p_sort2IdMap,
                    vector<float>& predVec,
					vector<int>& itemVec,
					vector<int>& feaVec,
					StaInfo* staInfo) {
 
    /*
    // 建树
    TreeNode* tmpRoot = new TreeNode();
    tmpRoot->depth = 0;
    TreeNode* root = _xgboostLoop(data, sortIdMap, predVec, tmpRoot, itemVec, feaVec);
    root->father = NULL;

    printTree(root);

    return root;
    */

    /*
    // 建树
    TreeNode* root = constructTreeNode(itemVec, &data, &predVec);
    root->depth = 0;

    vector<TreeNode*> levelList;
    vector<TreeNode*> newLevelList;
    levelList.push_back(root);

    int depth = 0;
    while (levelList.size() > 0) {
        depth += 1;
        printf("depth:%d\n", depth);

        int threadSize = levelList.size();
        struct Param params[threadSize];
        for (int i = 0; i < threadSize; ++i) {
            params[i].depth = depth;
            params[i].idx = i;
            params[i].nodes = &levelList;
            params[i].newNodes = NULL;
            params[i].data = &data;
            params[i].sortIdMap = &sortIdMap;
            params[i].feaVec = &feaVec;
            params[i].predVec = &predVec;
        }

        // printf("run 2.1\n");

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize (&attr, 50*1024*1024);    //修改栈大小为16M

        pthread_t threadPool[threadSize];
        for (int i = 0; i < threadSize; ++i) {
            int ret = pthread_create(&threadPool[i], &attr, threadNode, (void*)&params[i]);
            if (ret) {
                printf("thread create error!\n");
            }
        }
        for (int i = 0; i < threadSize; ++i) {
            pthread_join(threadPool[i], NULL);
        }

        newLevelList.clear();
        for (int i = 0; i < threadSize; ++i) {
            vector<TreeNode*>* _nodes = params[i].newNodes;
            for (int j = 0; j < _nodes->size(); ++j) {
                newLevelList.push_back(_nodes->at(j));
            }
        }
        // printf("run 2.2\n");

        levelList = newLevelList;

    }

    // printTree(root);
    */
    
    // 建树
    TreeNode* root = constructTreeNode(itemVec, data, &predVec);
    root->depth = 0;

    vector<TreeNode*> levelList;
    vector<TreeNode*> newLevelList;
    levelList.push_back(root);

	double _begin = getTime();
	int threadSize = 12;
	vector<float*> feaDataVec(threadSize);
	vector<float*> labelDataVec(threadSize);
	// vector<float**> _labelDataVec(threadSize);
	vector<float*> spVec(threadSize);
	vector<int8_t*>   itemMask(threadSize);
	for (int i = 0; i < threadSize; ++i) {
		feaDataVec[i]   = new float[itemVec.size()];
		labelDataVec[i] = new float[itemVec.size()];
		// _labelDataVec[i] = new float*[itemVec.size()];
		spVec[i]        = new float[itemVec.size()];
		itemMask[i]     = new int8_t[itemVec.size()];
	}
    vector<int>*   minFea  = new vector<int>(itemVec.size(), -1);
    vector<float>* minSp   = new vector<float>(itemVec.size(), FLT_MAX);
    vector<float>* minVar  = new vector<float>(itemVec.size(), FLT_MAX);
    vector<float>* improve = new vector<float>(itemVec.size(), 0.0f);

	printf("new:%lf\n", getTime() - _begin);

	// pthread_mutex_init(&mutex,NULL);//初始化互斥锁

    int depth = 0;
    while (levelList.size() > 0) {
        depth += 1;
        printf("depth:%d\n", depth);

        // int threadSize = 12;

        double begin = getTime();

        /*
        // if (levelList.size() <= 1000000000) {
        if (levelList.size() <= 1) {
            // levelWise(data, sortIdMap, predVec, itemVec, feaVec, threadSize, depth, levelList, newLevelList);
            levelWiseV2(data, sortIdMap, predVec, itemVec, feaVec, threadSize, depth, levelList, newLevelList);
        } else {
            levelThread(data, sortIdMap, predVec, itemVec, feaVec, threadSize, depth, levelList, newLevelList);
        }
        */

        // levelWiseV3(data, sortIdMap, predVec, itemVec, feaVec, threadSize, depth, levelList, newLevelList);
        // levelWiseV3(data, sortIdMap, predVec, itemVec, feaVec, feaDataVec, labelDataVec, _labelDataVec, spVec, itemMask, minFea, minSp, minVar, threadSize, depth, levelList, newLevelList);
        levelWiseV3(data, sortIdMap, p_sortIdMap, p_sort2IdMap, staInfo, predVec, itemVec, feaVec, feaDataVec, labelDataVec, spVec, itemMask,
			      	minFea, minSp, minVar, improve, threadSize, depth, levelList, newLevelList);

        printf("depth:%d spend:%lf node_size:%ld\n", depth, getTime() - begin, levelList.size());

        /*
        newLevelList.clear();
        for (int i = 0; i < threadSize; ++i) {
            vector<TreeNode*>* _nodes = params[i].newNodes;
            for (int j = 0; j < _nodes->size(); ++j) {
                newLevelList.push_back(_nodes->at(j));
            }
        }
        */
        // printf("run 2.2\n");

        levelList = newLevelList;
        newLevelList.clear();
    }

	for (int i = 0; i < threadSize; ++i) {
		delete[] feaDataVec[i];
		delete[] labelDataVec[i];
		delete[] spVec[i];
		delete[] itemMask[i];
	}
	delete minFea;
	delete minSp;
	delete minVar;

	// pthread_mutex_destroy(&mutex);

    return root;
}


#endif

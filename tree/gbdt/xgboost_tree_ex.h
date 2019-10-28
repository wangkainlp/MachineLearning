#ifndef _xgboost_tree_h_
#define _xgboost_tree_h_

#include <vector>
#include <map>
#include "tree_node.h"
#include "data.h"
#include "loss.h"


void findBestSpV2_2(Matrix* data,
                    map<int, vector<int> >& sortIdMap,
                    vector<float> predVec,
                    vector<int>& itemVec,
                    vector<int>& feaVec,
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
    float G  = 0.0f
    float H  = 0.0f;
    for (size_t j = 0; j < itemVec.size(); ++j) {
        for (int k = 0; k < itemVec[j]; ++k) {
            float pred  = predVec[j];
            float label = data.at(j, rangeSize - 1);
            G += pred - label;
            H += MAX(pred * (1.0f - pred), eps);
        }
    }

    for (size_t i = 0; i < feaVec.size(); ++i) {
        if (feaVec[i] < 1) { continue; }

        int fea = (int)i;

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
            if (last < spVec[spIdx] && spVec[spIdx] <= data.at(id, fea) ) {
                GR = G - GL;
                HR = H - HL;
                float curScore = (GL * GL) / (HL + LAMBDA) + (GR * GR) / (HR + LAMBDA) - (G * G) / (H + LAMBDA);
                printf("curFea:%d, curSp:%f, curScore:%f\n", fea, spVec[spIdx], curScore);
                if (curScore > curFeaMaxScore) {
                    curFeaMaxScore = curScore;
                    curFeaMaxSp = spVec[spIdx];
                    // curFeaMaxSp = feaSortDataVec[j];
                }
                ++spIdx;
                last = data.at(id, fea);
            }
            float pred  = predVec[id];
            float label = data.at(id, rangeSize - 1);
            GL += pred - label;
            HL += MAX(pred * (1.0f - pred), eps);
            // for (size_t k = 0; k < itemVec[id]; ++k) {
            //     float pred  = predVec[id];
            //     float label = data.at(id, rangeSize - 1);
            //     GL += pred - label;
            //     HL += MAX(pred * (1.0f - pred), eps);
            // }
        }

        printf("curFea:%d, curMaxScore:%f maxScore:%f\n", fea, curFeaMaxScore, maxScore);
        if (curFeaMaxScore > maxScore) {
            maxFea = fea;
            maxSp  = curFeaMaxSp;
            maxScore = curFeaMaxScore;
            printf("curScore:%f maxScore:%f\n", curFeaMaxScore,maxScore);
        }
        printf("run f n\n");
    }

    printf("maxfea: %d %.3f %.3f\n", maxFea, maxSp, maxScore);
}

void _findBestSpV2(Matrix& data, map<int, vector<int> >& sortIdMap,
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

void _findBestSpV2_1(Matrix& data, map<int, vector<int> >& sortIdMap,
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

void _findBestSpV2(Matrix& data, map<int, vector<int> >& sortIdMap,
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

TreeNode* constructNode(Matrix* data, vector<int>* itemVec, vector<float>* predVec) {

    TreeNode* node = new TreeNode();
    node->father = NULL; 
    node->left = NULL;
    node->right = NULL;

    node->size = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        node->size += itemVec[i];
    }

    node->father = root;

    node->size = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) { node->size += itemVec[i]; }

    node->itemIds = (int*)malloc(sizeof(int) * node->size);
    int idx = 0;
    for (size_t i = 0; i < itemVec.size(); ++i) {
        for (int j = 0; j < itemVec[i]; ++j) {
            node->itemIds[idx] = (int)i;
            ++idx;
        }
    }

    float G  = 0.0f, H  = 0.0f;
    for (int j = 0; j < node->size; ++j) {
        int _id = node->itemIds[j];
        float pred  = predVec[_id];
        float label = data.at(_id, data.getRange() - 1);
        float g = pred - label;
        float h = pred * (1.0f - pred);
        G += g;
        H += h;
    }

    node->value = -(G / (H + LAMBDA));
    // ->value = 1.0f / (1.0f + exp(-1.0f * w));
    // node->depth = root->depth + 1;

    return node;
}


TreeNode* levelWise(Matrix& data, map<int, vector<int> >& sortIdMap, vector<float>& predVec, vector<TreeNode*>& nodes,
                      vector<int>& itemVec, vector<int>& feaVec) {

    time_t begin = time(NULL);
    printf("depth: %d\n", root->depth);

    for (size_t _i = 0; _i < nodes.size(); ++_i) {
        TreeNode* node = nodes[_i];

        // 最大深度
        if (node->depth >= trainConf.maxDepth) { continue; }

        // 节点覆盖的最小数据量
        if (node->size <= trainConf.minCoverNum) { continue; }

        int minFea = -1;
        float minSp = FLT_MAX;
        float minVar = FLT_MAX;

        // printf("loop1:%ld\n", time(NULL) - begin);

        // _findBestSpV2_1(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
        // _findBestSpV2(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
        // findBestSpV1(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
        // findBestSp(data, sortIdMap, itemVec, feaVec, minFea, minSp, minVar);
        findBestSpV2_2(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
        // printf("minFea: %d %.3f %.3f\n", minFea, minSp, minVar);

        // printf("loop2:%ld\n", time(NULL) - begin);


        for (size_t i = 0; i < feaVec.size(); ++i) {
            if (feaVec[i] <= 0) { continue; }

            int fea = (int)i;



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

            int spIdx = 0;
            float last_value = 0.0f;
            feaDataVec_p = feaDataVec + 1;
            last_value = itBlock[0].fea;
            for (int j = 1; j < itemSize; ++j) {
                if (last_value + 1e-7 < itBlock[j].fea) {
                    spVec[spIdx] = 0.5 * (last_value + itBlock[j].fea);
                    ++spIdx;
                    last_value = itBlock[j].fea;
                }
                ++feaDataVec_p;
            }

            /*
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
                    // printf("%f ", spVec.at(spVec.size() - 1));
                }
            }
            printf("\n");
            */

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
                if (last < spVec[spIdx] && spVec[spIdx] <= data.at(id, fea) ) {
                    GR = G - GL;
                    HR = H - HL;
                    float curScore = (GL * GL) / (HL + LAMBDA) + (GR * GR) / (HR + LAMBDA) - (G * G) / (H + LAMBDA);
                    printf("curFea:%d, curSp:%f, curScore:%f\n", fea, spVec[spIdx], curScore);
                    if (curScore > curFeaMaxScore) {
                        curFeaMaxScore = curScore;
                        curFeaMaxSp = spVec[spIdx];
                        // curFeaMaxSp = feaSortDataVec[j];
                    }
                    ++spIdx;
                    last = data.at(id, fea);
                }
                float pred  = predVec[id];
                float label = data.at(id, rangeSize - 1);
                GL += pred - label;
                HL += MAX(pred * (1.0f - pred), eps);
                // for (size_t k = 0; k < itemVec[id]; ++k) {
                //     float pred  = predVec[id];
                //     float label = data.at(id, rangeSize - 1);
                //     GL += pred - label;
                //     HL += MAX(pred * (1.0f - pred), eps);
                // }
            }

            printf("curFea:%d, curMaxScore:%f maxScore:%f\n", fea, curFeaMaxScore, maxScore);
            if (curFeaMaxScore > maxScore) {
                maxFea = fea;
                maxSp  = curFeaMaxSp;
                maxScore = curFeaMaxScore;
                printf("curScore:%f maxScore:%f\n", curFeaMaxScore,maxScore);
            }
     





        // 没有分裂点
        if (minFea < 0 ) { continue; }

        // 增益太小
        if (minVar < trainConf.minGain) { continue; }

        gFScore[minFea] += 1;
        gGain[minFea]   += minVar;
        gCover[minFea]  += H;

        node->fidx = minFea;
        node->split = minSp;

        vector<int> leftItems;
        leftItems.reserve(trainConf.dataSize);
        vector<int> rightItems;
        rightItems.reserve(trainConf.dataSize);

        for (size_t i = 0; i < itemVec.size(); ++i) {
            if (itemVec[i] > 0 && data.at(i, minFea) < minSp) {
                leftItems.push_back(itemVec[i]);
            } else {
                rightItems.push_back(itemVec[i]);
            }
        }

        // 无法继续划分
        if (leftItems.size() <= 0 || rightItems.size() <= 0) {
            printf("左子树或右子树为空: %d\t%f\t%f\n", minFea, minSp, minVar);
        }

        node->left  = constructNode(leftItems, data, &predVec);
        node->right = constructNode(leftItems, data, &predVec);
        node->left->depth = node->depth + 1;
        node->right->depth = node->depth + 1;
    
        newLevelList.push_back(node->left);
        newLevelList.push_back(node->right);

    }

    return node;
}



TreeNode* _xgboostLoop(Matrix& data, map<int, vector<int> >& sortIdMap, vector<float>& predVec, TreeNode* root,
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
    _findBestSpV2_1(data, sortIdMap, predVec, itemVec, feaVec, minFea, minSp, minVar);
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

    newNode->left  = _xgboostLoop(data, sortIdMap, predVec, newNode, leftItems, feaVec);
    newNode->right = _xgboostLoop(data, sortIdMap, predVec, newNode, rightItems, feaVec);

    return newNode;
}

int _comp(const pair<int, float>& a, const pair<int, float>& b) {
    return a.second < b.second;
}

TreeNode* _xgboost_main(Matrix& data, vector<float>& predVec, vector<int>& itemVec, vector<int>& feaVec) {

    // 预排序
    map<int, vector<int> > sortIdMap;

    for (size_t _i = 0; _i < feaVec.size(); ++_i) {
        int fea = _i;

        vector<pair<int, float> > pairIdFeaVec;
        for (int i = 0; i < data.length(); ++i) {
            pairIdFeaVec.push_back(make_pair(i, data.at(i, fea)));
        }

        sort(pairIdFeaVec.begin(), pairIdFeaVec.end(), _comp);

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

    /*
    // 建树
    TreeNode* tmpRoot = new TreeNode();
    tmpRoot->depth = 0;
    TreeNode* root = _xgboostLoop(data, sortIdMap, predVec, tmpRoot, itemVec, feaVec);
    root->father = NULL;

    printTree(root);
    */

    TreeNode* root = new TreeNode();
    constructNode(data, itemVec, predVec);
    root->depth = 1;
    root->father = root;

    TreeNode* levelList = new vector<TreeNode*>();
    TreeNode* newLevelList = new vector<TreeNode*>();
    levelList.append(root);

    int depth = 0;
    while (levelList.size() > 0) {
        depth += 1;

        levelWise(data, sortIdMap, predVec, levelList, itemVec, feaVec);

        levelList = newLevelList;
        newLevelList.clear();
    }

    return root;
}


#endif

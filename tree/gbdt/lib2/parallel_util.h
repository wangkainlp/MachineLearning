#include <pthread.h>

#include <map>
#include "data.h"
#include "gbdt_tree.h"
#include "tree_node.h"

#ifndef __parallel_util__
#define __parallel_util__

struct ParamP {
    Matrix* data;
    map<int, Block*>* p_sortIdMap;
    int dim;
    float shrink;
    float* preds;
    float* pre_preds;
    int groupId;
    int step;
    TreeNode* root;
};

void* threadPredict(void* paramPtr) {
    struct ParamP* param = (struct ParamP*)paramPtr;
    Matrix* data = param->data;
    float* preds = param->preds;
    float* pre_preds = param->pre_preds;
    int dim = param->dim;
    int groupId = param->groupId;
    int step = param->step;
    float shrink = param->shrink;
    TreeNode* root = param->root;

    int start = groupId * step;
    int end = (groupId + 1)* step;
    end = end > data->length() ? data->length() : end;

    for (int i = start; i < end; ++i) {
        float* feas = data->getRow(i);
        float pred = predict(root, feas);
        pre_preds[i] += pred;

        // pred = predTransform(pred);

        pred *= shrink;

        preds[i] = data->at(i, dim + 1) - pred;
    }
    // printf("pred func end:%d\n", groupId);
    return static_cast<void*>(0);
}

int parallel_predict(Matrix* data,
                     TreeNode* root,
                     int threadSize,
                     int dim,
                     float shrink,
                     vector<float>& pre_predVec,
                     vector<float>& predVec) { 
    int dataLen = predVec.size();
    int step = (dataLen + threadSize - 1) / threadSize;

    struct ParamP params[threadSize];
    float* preds = new float[predVec.size()];

    float* pre_preds = new float[pre_predVec.size()];
    for (int i = 0; i < pre_predVec.size(); ++i) {
        pre_preds[i] = pre_predVec[i];
    }

    for (int i = 0; i < threadSize; ++i) {
        params[i].data = data;
        params[i].dim = dim;
        params[i].shrink = shrink;
        params[i].groupId = i;
        params[i].step = step;
        params[i].preds = preds;
        params[i].pre_preds = pre_preds;
        params[i].root = root;
    }

    pthread_t threadPool[threadSize];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize (&attr, 5*1024*1024);    //修改栈大小

    for (int i = 0; i < threadSize; ++i) {
        int ret = pthread_create(&threadPool[i], &attr, threadPredict, (void*)&params[i]);
        if (ret) {
            fprintf(stderr, "create thread error!\n");
            return 1;
        }
    }

    for (int i = 0; i < threadSize; ++i) {
        void* status;
        pthread_join(threadPool[i], &status);
    }
    
    for (int i = 0; i < predVec.size(); ++i) {
        predVec[i] = preds[i];
        pre_predVec[i] = pre_preds[i];
    }
    delete[] preds;
    delete[] pre_preds;
    // printf("pred end\n");
    return 0;
}

void* func_refresh(void* paramPtr) {

    struct ParamP* param = (struct ParamP*)paramPtr;
    Matrix* data = param->data;
    float* preds = param->preds;
    int dim = param->dim;
    int groupId = param->groupId;
    int step = param->step;
    int dataLen = data->length();
    map<int, Block*>* p_sortIdMap = param->p_sortIdMap;

    int start = groupId * step;
    int end = (groupId + 1)* step;
    end = end > dim ? dim : end;

    for (int i = start; i < end; ++i) {
        Block* sordIds = (*p_sortIdMap)[i];
        for (int j = 0; j < dataLen; ++j) {
            sordIds[j].label = preds[sordIds[j].id];
        }
    }
    return static_cast<void*>(0);
}


int parallel_refresh(Matrix* data, map<int, Block*>* p_sortIdMap, int dim, int threadSize, vector<float>& predVec) {

    int step = (dim + threadSize - 1) / threadSize;

    float* preds = new float[predVec.size()];
    for (int i = 0; i < predVec.size(); ++i) {
        preds[i] =  predVec[i];
    }

    struct ParamP params[threadSize];
    for (int i = 0; i < threadSize; ++i) {
        params[i].data = data;
        params[i].p_sortIdMap = p_sortIdMap;
        params[i].preds = preds;
        params[i].dim = dim;
        params[i].groupId = i;
        params[i].step = step;
    }

    pthread_t threadPool[threadSize];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize (&attr, 5*1024*1024);    //修改栈大小

    for (int i = 0; i < threadSize; ++i) {
        int ret = pthread_create(&threadPool[i], &attr, func_refresh, (void*)&params[i]);
        if (ret) {
            fprintf(stderr, "create thread error!\n");
            return 1;
        }
    }

    for (int i = 0; i < threadSize; ++i) {
        void* status;
        pthread_join(threadPool[i], &status);
    }

    delete[] preds;
    
    return 0;
}

#endif

#ifndef _loss_h_
#define _loss_h_

#include <math.h>

#include "common.h"

// #define MAX(x, y) ( x > y ? x : y )

const float EPSLON = 1e-16f;

float sigmoid(float x) {
    return 1.0f / (1.0f + expf(-x));
}

class Loss {
public:
    virtual float firstOrderGradient(float pred, float label) = 0;
    virtual float secondOrderGradient(float pred, float label) = 0;
    virtual float predTransform(float x) = 0;
};

class SquareLoss : Loss {
public:
    float firstOrderGradient(float pred, float label) {
        return pred - label;
    }
    float secondOrderGradient(float pred, float label) {
        return 1.0f;
    }
    float predTransform(float x) {
        return x;
    }
};

class LogisticLoss : Loss {
public:
    float firstOrderGradient(float pred, float label) {
        return pred - label;
    }
    float secondOrderGradient(float pred, float label) {
        return MAX(pred * (1.0f - pred), EPSLON);
    }
    float predTransform(float x) {
        return sigmoid(x);
    }

private:
    float clip(float x) {
        if (x > 0.99999f) {
            return 0.99999f;
        } else if (x < 0.000001f) {
            return 0.0000001f;
        } else {
            return x;
        }
    }
};

#endif

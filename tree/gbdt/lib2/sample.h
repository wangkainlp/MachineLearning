#include <cstdlib>
#include <vector>


#ifndef _sample_h_
#define _sample_h_

using namespace std;

vector<int> rowSample(int n, float sample_rate) {
    vector<int> mask;
    mask.resize(n);
    for (int i = 0; i < n; ++i) { 
        mask[i] = 0;
    }
    // 放回采样
    for (int i = 0; i < n; ++i) { 
        // int id = (int)((double)rand() / RAND_MAX * n);
        // mask[id] += 1;
        mask[i] += ((double)rand() / RAND_MAX) < sample_rate ? 1 : 0;
    }
    /*
    for (int i = 0; i < n; ++i) { 
        mask[i] = 1;
        printf("%d ", mask[i] );
    }
    printf("\n");
    */
    return mask;
}

vector<int> columnSample(int n, double sample_rate) {
    vector<int> mask;
    mask.resize(n);
    for (int i = 0; i < n; ++i) { 
        mask[i] = ((double)rand() / RAND_MAX) < sample_rate ? 1 : 0;
    }
    /*
    for (int i = 0; i < n; ++i) { 
        printf("%d ", mask[i]);
    }
    printf("\n");
    */
    return mask;
}

#endif

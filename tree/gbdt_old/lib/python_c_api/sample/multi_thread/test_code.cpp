#include <iostream>
#include <vector>
using namespace std;

int main() {

    vector<int> feaVec;
    for (int i = 0; i < 100; ++i) {
        feaVec.push_back(i);
    }

    int max_thread_size = 7;
    int thread_size = 0;
    for (int i = 0; i < feaVec.size(); i += thread_size) {
        // int fea = feaVec[i];

        thread_size = max_thread_size;
        if (i + max_thread_size > feaVec.size()) {
            thread_size = feaVec.size() - i;
        }

        for (int j = 0; j < thread_size; ++j) {
            cout << feaVec[i + j] << endl;
        }

    }

    return 0;
}

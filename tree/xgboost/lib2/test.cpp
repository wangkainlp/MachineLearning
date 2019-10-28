#include <vector>
#include <iostream>
#include <cstdio>
using namespace std;
int main() {

    vector<float>* vec = new vector<float>(5, 0.5f);
    cout << vec->size() << endl;
    for (int i = 0; i < vec->size(); ++i) {
        cout << vec->at(i) << endl;
    }
    for (int i = 0; i < vec->size(); ++i) {
        printf("%lf\n", vec->at(i));
    }

    return 0;
}

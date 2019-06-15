#include <vector>
#include <iostream>
using namespace std;
int main() {

    vector<float>* vec = new vector<float>(5, 2.1);
    cout << vec->size() << endl;
    for (int i = 0; i < vec->size(); ++i) {
        cout << vec->at(i) << endl;
    }


    return 0;
}

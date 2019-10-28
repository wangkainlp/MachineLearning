#include <iostream>
#include <unistd.h>
#include <vector>
#include <pthread.h>
using namespace std;

struct M {
    int id;
    int value;
    vector<int>& vec;
};

void in_func(int& n) {
    n = 100;
}

void* thread (void* ptr) {
    /*
    vector<int> vec = *((vector<int>*)ptr);
    cout << "This is a thread:" << vec[0] << endl;
    vec[0] = 100;
    */
    
    /*
    struct M* num = (struct M*)(ptr);
    for (int i = 0; i < num->value; ++i) {
        in_func(i);
        sleep(1);
        cout << "This is a thread:" << num->id << endl;
    }
    */

    struct M* param = (struct M*)(ptr);
    cout << "This is a thread:" << param->id << "vec:" << param->vec[0] << endl;

    
    return 0;
}

int main() {

    vector<int> vec;
    vec.push_back(1);

    struct M param[5];
    pthread_t id[5];
    for (int i = 0; i < 5; ++i) {
        param[i].value = 2;
        param[i].id = i;
        cout << param[i].id << endl;
        int ret = pthread_create(&id[i], NULL, thread, (void *)&param[i]);
        if (ret == 1) {
            cout << "create thread error!" << endl;
            return 1;
        }
    }

    for (int i = 0; i < 3; ++i) {
        cout << "This is main process." << endl;
        sleep(1);
    }

    for (int i = 0; i < 5; ++i) {
        void* status;
        // pthread_join(id, NULL);
        pthread_join(id[i], &status);
        cout << "return:" << (long)status << endl;
    }
    
    cout << vec[0] << endl;

    return 0;
}

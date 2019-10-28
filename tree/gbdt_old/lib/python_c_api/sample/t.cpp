#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <vector>
using namespace std;

#define FLT_MAX         3.402823466e+38F

int main() {
    time_t t = time(NULL);
    cout << t << endl;
    cout << time(NULL) << endl;
    cout << system("time") << endl;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    double mm = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
    printf("%lf\n", mm);

    cout << "----------------------------" << endl;

    vector<int> vec1;
    vec1.push_back(1);
    vec1.push_back(2);

    vector<int> vec2;
    vec2.push_back(5);
    vec2.push_back(6);

    vec1.insert(vec1.end(), vec2.begin(), vec2.end());
    vec1.assign(vec2.begin(), vec2.end());

    for (int i = 0; i < vec1.size(); ++i) {
        cout << vec1[i] << endl;
    }

    cout << "----------------------------" << endl;

    float a = FLT_MAX;
    float b = FLT_MAX;

    float c = a;

    if (c < b) {
        printf("less %f\n", b - c );
    }

    cout << a - c << endl;
    printf("%f\n", a - c );

    cout << "----------------------------" << endl;

    int* arr = new int(5);

    

    return 0;
}

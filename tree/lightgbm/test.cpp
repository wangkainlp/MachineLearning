#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

class HistogramBinEntry {
public:
    double sum_gradients = 0.0f;    // sum of gradients on this bin
    double sum_hessians = 0.0f;     // sum of hessian on this bin
    data_size_t cnt = 0;            // number of data on this bin
};

bool DoubleEqual(double a, double b) {

    double eps = 1e-7;
    double sub = a - b;
    if (sub <= eps && sub >=-eps) {
        return true;
    } else {
        return false;
    }
}

vector<double> greedyFindBin(vector<double> distinctValues, vector<int> counts, int maxBin, int minDataInBin) {

    vector<double> bin_upper_bound;
    if (distinctValues.size() < maxBin) {
        int cnt = 0;
        for (size_t i = 0; i < distinctValues.size() - 1; ++i) {
            cnt += counts[i];
            if (cnt >= minDataInBin) {
                double val = (distinctValues[i] + distinctValues[i + 1]) / 2.0;
                if (bin_upper_bound.empty() || !DoubleEqual(bin_upper_bound.back(), val)) {
                    bin_upper_bound.push_back(val);
                    cnt = 0;
                }
            }
        }
        bin_upper_bound.push_back(std::numeric_limits<double>::infinity());
    } else {
        int totalSize = 0;
        for (size_t i = 0; i < distinctValues.size(); ++i) {
            totalSize += counts[i];
        }
        int meanBinSize = (int)(totalSize / distinctValues.size());
        printf("meanBinSize:%d\n", meanBinSize);

        vector<bool> needOneBin(distinctValues.size(), false);
        for (size_t i = 0; i < distinctValues.size(); ++i) {
            if (counts[i] >= meanBinSize) {
                needOneBin[i] = true;
            }
        }

        vector<double> binLowerBound(maxBin, -numeric_limits<double>::infinity());
        vector<double> binUpperBound(maxBin, numeric_limits<double>::infinity());
        int inBinCnt = 0;
        int binCnt = 0;
        for (size_t i = 0; i < distinctValues.size() - 1; ++i) {
            inBinCnt += counts[i];
            if (inBinCnt >= minDataInBin || needOneBin[i] || (needOneBin[i + 1] && inBinCnt >= (meanBinSize / 2))) {
                binUpperBound[binCnt] = distinctValues[i];
                ++binCnt;
                binLowerBound[binCnt] = distinctValues[i + 1];

                printf("inBinCnt:%d needOneBin:%d\n", inBinCnt, (int)needOneBin[i]);

                inBinCnt = 0;
                if (binCnt >= maxBin - 1) {
                    break;
                }
            }
        }

        ++binCnt;
        for (int i = 0; i < binCnt - 1; ++i) {
            double val = (binUpperBound[i] + binLowerBound[i + 1]) / 2.0;
            if (bin_upper_bound.empty() || !DoubleEqual(bin_upper_bound.back(), val)) {
                bin_upper_bound.push_back(val);
            }
        }
        bin_upper_bound.push_back(numeric_limits<double>::infinity());
        
        for (int i = 0; i < binCnt; ++i) {
            printf("[%lf, %lf]\n", binLowerBound[i], binUpperBound[i]);
        }
    }

    for (size_t i = 0; i < bin_upper_bound.size(); ++i) {
        cout << setprecision(10) << bin_upper_bound[i] << ", " ;
    }
    cout << endl;
    return bin_upper_bound;
}

struct BinEntry {
    double sumGradients;
    double sumHessians;
    int cnt;
};
void constructHistogram() {
    vector<HistogramBinEntry> histogram(maxBin);
    
    for (int i = 0; i < data_size; ++i) {
        int bin = data_[i];
        histogram[bin].sum_gradients += gradients[i];
        histogram[bin].sum_hessians += hessians[i];
        histogram[bin].cnt += 1;
    }
}

void test_greedyFindBin() {
    double data[]  = {1.0, 2.0, 2.00000001, 2.000000001,  5.0};
    int    count[] = {9,   1,   1,   1,   1 };
    int minDataInBin = 4;
    int maxBin = 3;

    vector<double> distinctValues;
    vector<int> counts;
    for (int i = 0; i < sizeof(data) / sizeof(double); ++i) {
        distinctValues.push_back(data[i]);
        counts.push_back(count[i]);
        cout << data[i] << ", ";
    }
    cout << endl;
 
    vector<double> bin_upper_bound = greedyFindBin(distinctValues, counts, maxBin, minDataInBin);

    int data_size = sizeof(data) / sizeof(double);
    vector<int> dataIndices;
    vector<int> data_;
    data_.resize(data_size);
    for (int i = 0; i < data_size; ++i) {
        for (size_t j = 0; j < bin_upper_bound.size(); ++j) {
            if (distinctValues[i] < bin_upper_bound[j]) {
                data_[i] = j;
            }
        }
    }



}

void test_greedyFindBin() {
    double data[]  = {1.0, 2.0, 2.00000001, 2.000000001,  5.0};
    int    count[] = {9,   1,   1,   1,   1 };
    int minDataInBin = 4;
    int maxBin = 3;

    vector<double> distinctValues;
    vector<int> counts;
    for (int i = 0; i < sizeof(data) / sizeof(double); ++i) {
        distinctValues.push_back(data[i]);
        counts.push_back(count[i]);
        cout << data[i] << ", ";
    }
    cout << endl;
    greedyFindBin(distinctValues, counts, maxBin, minDataInBin);

}

int main() {
    test_greedyFindBin();

    return 0;
}

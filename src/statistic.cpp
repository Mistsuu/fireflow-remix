#include <vector>
#include <map>
#include "packet.h"

#define loop(_i_, _n_)      for(int _i_ = 0;   _i_ <  _n_; ++_i_)
#define lrlp(_i_, _l_, _r_) for(int _i_ = _l_; _i_ <= _r_; ++_i_)

using namespace std;

/*
    updateEntropyAndMap():
        Update entropy and map from new data.
*/
template<typename T>
void updateEntropyAndMap(map<T, int>& countMap, double& entropy, T* pNewData, T* pOldData, int& windowSize) {
    if (!countMap.count(*pNewData)) countMap[*pNewData] = 1;
    else {
        countMap[*pNewData]++;
        entropy -= (countMap[*pNewData]  ) * log2(countMap[*pNewData]  ) / windowSize;
        entropy += (countMap[*pNewData]-1) * log2(countMap[*pNewData]-1) / windowSize;
    }

    if      (pOldData && !countMap.count(*pOldData)) assert(false);
    else if (pOldData &&  countMap.count(*pOldData)) countMap[*pOldData]--;
    if      (pOldData && !countMap[*pOldData]      ) countMap.erase(*pOldData);
    else if (pOldData &&  countMap[*pOldData]      ) {
        entropy -= (countMap[*pNewData]  ) * log2(countMap[*pNewData]  ) / windowSize;
        entropy += (countMap[*pNewData]+1) * log2(countMap[*pNewData]+1) / windowSize;
    }
}

/*
    getMean():
        Get mean from a list of data.
*/
double getMean(vector<double>& input) {
    double mean=0;
    loop(i, input.size())
        mean += input[i];
    return mean / input.size();
}

/*
    CUSUM():
        Performs CUSUM algorithm.
*/
void CUSUM(vector<double>& input, vector<double>& output, double beta) {
    // Clear output first, if necessary.
    output.clear();

    // Zn = Xn - beta
    output.push_back(0);
    loop(i, input.size())
        output.push_back(input[i] - beta);

    // yn = (yn-1 + Zn)+
    lrlp(i, 1, input.size()) {
        output[i] += output[i-1];
        output[i]  = (output[i]>0 ? output[i]:0);
    }
}
#include <vector>
#include <map>
#include <cmath>
#include "packet.h"

#define loop(_i_, _n_)      for(int _i_ = 0;   _i_ <  _n_; ++_i_)
#define lrlp(_i_, _l_, _r_) for(int _i_ = _l_; _i_ <= _r_; ++_i_)

using namespace std;

template<class T>
void update(map<T, int>& countMap, double& entropy, T* pNewData, T* pOldData, int& windowSize) {
    
    if (pNewData) { // Might add more constraints later on, and the rule still holds.
        if (!countMap.count(*pNewData)) countMap[*pNewData] = 1;
        else {
            countMap[*pNewData]++;
            entropy -= (countMap[*pNewData]  ) * log2(countMap[*pNewData]  ) / windowSize;
            entropy += (countMap[*pNewData]-1) * log2(countMap[*pNewData]-1) / windowSize;
        }
        entropy += log2(windowSize) / windowSize;
    }

    if (pOldData) { // Might add more constraints later on, and the rule still holds.
        countMap[*pOldData]--;
        if (countMap[*pOldData] == 0 ) countMap.erase(*pOldData);
        else {
            entropy -= (countMap[*pOldData]  ) * log2(countMap[*pOldData]  ) / windowSize;
            entropy += (countMap[*pOldData]+1) * log2(countMap[*pOldData]+1) / windowSize;
        }
        entropy -= log2(windowSize) / windowSize;
    }

}

template void update<uint32_t>(map<uint32_t, int>& countMap, double& entropy, uint32_t* pNewData, uint32_t* pOldData, int& windowSize);
template void update<uint16_t>(map<uint16_t, int>& countMap, double& entropy, uint16_t* pNewData, uint16_t* pOldData, int& windowSize);

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
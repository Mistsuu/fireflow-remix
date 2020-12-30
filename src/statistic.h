#ifndef STATISTIC_HEADER

    #define STATISTIC_HEADER

    #include <vector>
    #include <map>
    using namespace std;

    template<class T>
    void   updateEntropyAndMap(map<T, int>& countMap, double& entropy, T* pNewData, T* pOldData, int& windowSize);
    double getMean(vector<double>& input);
    void   CUSUM(vector<double>& input, vector<double>& output, double beta);

#endif
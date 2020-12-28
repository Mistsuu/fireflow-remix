#ifndef STATISTIC_HEADER

    #define STATISTIC_HEADER

    #include <vector>
    using namespace std;

    double getMean(vector<double>& input);
    void   CUSUM(vector<double>& input, vector<double>& output, double beta);

#endif
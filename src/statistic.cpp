#include <vector>

#define loop(_i_, _n_)      for(int _i_ = 0;   _i_ <  _n_; ++_i_)
#define lrlp(_i_, _l_, _r_) for(int _i_ = _l_; _i_ <= _r_; ++_i_)

using namespace std;

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
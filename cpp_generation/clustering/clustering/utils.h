#ifndef UTILS_H
#define UTILS_H
#include <time.h>
#include <random>
#include "clustering.h"
#include <vector>
#include <cmath>

double dist(const double* p1, const double* p2, Config* config);
bool same_points(double* p1, double* p2, Config* config);
double prandom(double min, double max);
long get_seed(int loop_index);
double median(std::vector<double> values);
template <typename T>
void print_array(T* array, int count)
{
    using namespace std;
    cout << "[ ";
    for (int i = 0; i < count; i++) {
        cout << array[i] << ",";
    }
    cout << endl;
}

double binom_fn(int n, int k);
#endif
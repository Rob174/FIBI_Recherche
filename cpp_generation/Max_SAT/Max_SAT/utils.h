#ifndef __UTILS_H__
#define __UTILS_H__
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <math.h>
#include <sstream>
std::wstring to_wstring_p(std::string s);

std::string to_string(const char *v);
int binomial_coeff(int k, int n);
template <typename T, typename Tout>
Tout *copy_array(T *array, int size)
{
    Tout *new_array = new Tout[size];
    for (int i = 0; i < size; i++)
    {
        new_array[i] = array[i];
    }
    return new_array;
}
// equivalent of python join
std::string join(std::vector<std::string> texts, std::string separator = " ");
template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}
double dist(const double *p1, const double *p2, const int num_dim);
double dist_squared(const double* p1, const double* p2, const int num_dim);
template <typename T>
T max(T v1, T v2) {
    if (v1 > v2) {
        return v1;
    }
    else {
        return v2;
    }
}
template <typename T>
T min(T v1, T v2) {
    if (v1 < v2) {
        return v1;
    }
    else {
        return v2;
    }
}
template <typename T>
T between(T start, T end, T value) {
    if(value < start) {
        return start;
    }
    else if(value > end) {
        return end;
    }
    else {
        return value;
    }
}
#endif // __UTILS_H__
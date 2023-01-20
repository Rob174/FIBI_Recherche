#include "utils.h"

std::string to_string(const char *v)
{
    return std::string(v);
}
int binomial_coeff(int k, int n)
{
    if (k > n)
    {
        return 0;
    }
    if (k == n)
    {
        return 1;
    }
    int *b = new int[n + 1];
    b[0] = 1;
    for (int k = 0; k < n; ++k)
    {
        b[k + 1] = (b[k] * (n - k)) / (k + 1);
    }
    int v = b[n];
    return v;
}
std::string join(std::vector<std::string> texts, std::string separator)
{
    bool start = true;
    std::string out = "";
    for (std::string s : texts)
    {
        if (!start)
        {
            out.append(separator);
        }
        out.append(s);
        start = false;
    }
    return out;
}

std::wstring to_wstring_p(std::string s)
{
    return std::wstring(s.begin(), s.end());
}
double dist(const double *p1,const double *p2, const int num_dim)
{
    return sqrt(dist_squared(p1,p2, num_dim));
}
double dist_squared(const double* p1, const double* p2, const int num_dim)
{
    double d = 0;
    for (int i = 0; i < num_dim; i++)
    {
        const double v = (p1[i] - p2[i]);
        d += v * v;
    }
    return d;
}
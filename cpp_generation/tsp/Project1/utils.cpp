#include "utils.h"

double dist_squared(double* p1, double* p2, Config* config)
{
    double dist = 0.0;
    for (int i = 0; i < config->NUM_DIM; i++)
    {
        dist += (p1[i] - p2[i])* (p1[i] - p2[i]);
    }
    return dist;
}
double dist(double* p1, double* p2, Config* config) {
    return sqrt(dist_squared(p1, p2, config));
}

bool same_points(double* p1, double* p2, Config* config)
{
    bool issame = true;
    for (int i = 0; i < config->NUM_DIM; i++) {
        if (p1[i] != p2[i]) {
            issame = false;
            break;
        }
    }
    return issame;
}

double prandom(double min, double max)
{
    return min + ((double)std::rand() / (double)(RAND_MAX)) * (max - min);
}
//FROM https://stackoverflow.com/questions/322938/recommended-way-to-initialize-srand
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a = a - b;  a = a - c;  a = a ^ (c >> 13);
    b = b - c;  b = b - a;  b = b ^ (a << 8);
    c = c - a;  c = c - b;  c = c ^ (b >> 13);
    a = a - b;  a = a - c;  a = a ^ (c >> 12);
    b = b - c;  b = b - a;  b = b ^ (a << 16);
    c = c - a;  c = c - b;  c = c ^ (b >> 5);
    a = a - b;  a = a - c;  a = a ^ (c >> 3);
    b = b - c;  b = b - a;  b = b ^ (a << 10);
    c = c - a;  c = c - b;  c = c ^ (b >> 15);
    return c;
}
long get_seed(int loop_index) {
    return mix(clock(), time(NULL)*100+loop_index, _getpid());
}
int cycle_id(int i, int length) {
    return (i % length + length) % length;
}
int cyclic_length(int i, int j,int length_cycle) {
    if (j < i)
        j = i + length_cycle;
    return j - i+1;
}
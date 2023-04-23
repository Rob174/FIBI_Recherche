#ifndef UTILS_H
#define UTILS_H
#include "constants.h"
#include <time.h>
#include <random>
#include <process.h>
#include <math.h>

double dist(double* p1, double* p2, Config* config);
double dist_squared(double* p1, double* p2, Config* config);
bool same_points(double* p1, double* p2, Config* config);
double prandom(double min, double max);
long get_seed(int loop_index);
int cycle_id(int i, int length);
int cyclic_length(int i, int j, int length_cycle);
#endif
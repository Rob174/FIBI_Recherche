#pragma once

#include <random>
#include <math.h>
#include "constants.h"

double* generate_gaussian(const double* center, const int num_points, const double* stds, Config* conf);
double* linspace_p(const double start, const double end, const int num_points);
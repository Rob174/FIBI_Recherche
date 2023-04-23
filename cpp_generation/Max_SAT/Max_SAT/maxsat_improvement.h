#ifndef MAXSAT_IMPROVEMENT_H
#define MAXSAT_IMPROVEMENT_H
#include "Container.h"
#include "constants.h"
#include <random>
#include <numeric>

struct stats {
    double SAT;
    double UNSAT;
    double B;
};
void improve(MAXSATContainer*cont,const int seed = 0);
#endif
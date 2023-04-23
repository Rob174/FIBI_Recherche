#pragma once
#include "preprocessing.h"
#include "constants.h"
#include <iomanip>
#include <math.h>
class Tour {
private:
    int* tour;
    int length;
public:
    Tour(int* init_tour, Config* conf);
    ~Tour();
    double get_cost(DistanceMatrix* dist);
    double get_length(DistanceMatrix* dist);
    void describe();
    void describe(int start_id, int stop_id);
    int at(int i, Config* conf);
    int check_tour();
    void apply_two_opt(int i, int j,Config*conf);
};
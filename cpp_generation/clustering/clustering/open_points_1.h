
#include <H5Cpp.h>
#include <string>
#include <list>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <random>
#include "constants.h"


using namespace H5;

double* open_points_1(int i, std::string filename, Config* config, bool load_num_clust = true);
#pragma once

//Windows hdf5 installation https://www.programmersought.com/article/2813194707/
#include <H5Cpp.h>
#include <string>
#include <list>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <random>
#include "constants.h"

using namespace H5;
const H5std_string FILE_NAME("tsplib.hdf5");

double* tsplib_points(std::string dataset_name, Config* config);
double* tsplib_sample_points(Config* config);
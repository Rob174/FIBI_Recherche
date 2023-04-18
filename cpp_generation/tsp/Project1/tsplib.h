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
#include <fstream>
#include <errno.h>

using namespace H5;
const H5std_string FILE_NAME("tsplib.hdf5");

double* tsplib_points(std::string dataset_name, const H5std_string FILE_NAME, Config* config);
double* tsplib_sample_points(const H5std_string FILE_NAME, Config* config);
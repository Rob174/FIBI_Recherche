#pragma once
#include "H5Cpp.h"
#include "results.h"
#include <string>
#include <list>
#include <iostream>
#include <filesystem>
#include<sstream>
using namespace H5;

void save_to_hdf5(Result* res, Config* conf);

#pragma once
#include "H5Cpp.h"
#include "clustering.h"
#include "results.h"
#include <string>
#include <list>
#include <iostream>
#include <filesystem>
using namespace H5;

void create_dataset(Result* res, Clustering* final_clust);

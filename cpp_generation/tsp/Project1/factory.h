#pragma once
#include "BI.h"
#include "FI.h"
#include "BI_NL.h"
#include "FI_NL.h"
#include "constants.h"
#include "improved_init_tour.h"
#include "random_init_tour.h"
#include "random_eucl.h"
#include "tsplib.h"
#include "preprocessing.h"
#include "save_to_hdf5.h"

void run_algorithm(Config* conf);
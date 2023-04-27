#pragma once
#include "constants.h"
#include "centroids.h"
#include "clustering.h"
#include "results.h"
// points init
#include "blobs.h"
#include "moon.h"
#include "uniform.h"
#include "tsplib.h"
#include "open_points_1.h"
// clustering init
#include "random_clust.h"
#include "kmeans.h"
#include "kmeansPlusPlus.h"
// algorithms
#include "iteration_order.h"
#include "FI.h"
#include "BI.h"
// save results
#include "dataset.h"
void run_algorithm(Config* conf);


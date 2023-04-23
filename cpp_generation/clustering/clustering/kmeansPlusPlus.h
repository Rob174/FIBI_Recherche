#pragma once

#include "constants.h"
#include "clustering.h"
#include "utils.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <set>
#include <list>
#include <map>

void kmeansPlusPlus(Clustering* clustering, Config* config);
void kmeansCentroids(Clustering* clustering, Config* config);
void kmeansPlusPlusSuperGlutton(Clustering* clustering, Config* config);
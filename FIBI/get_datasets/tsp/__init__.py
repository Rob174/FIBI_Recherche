"""
Two datasets are available for the TSP problem:

1. uniform towns randomly distributed in a square of side 100. 
2. TSPLIB instances.

## I. Uniform points

Generated directly by the C++ program with the provided SEED_PROBLEM.

The coordinates are generated using `std::uniform_real_distribution<double> distrib(0, 100)` for each coordinate.

## II. TSPLIB instances

TSPLIB instances are available at http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/index.html.

More precisely the list of the of each instances has been extracted with selenium (simply get the a href of each instance without any filter).

Then the FIBI.get_datasets.tsp.create_datasets.main() function has been used to download the instances, filter the euclidean instances with less than 1000 towns and store them in a hdf5 file.
"""

"""
3 datasets are available for the TSP problem:

1. uniform points randomly distributed in a square of side 100. 
2. Franti et al benchmark instances.
3. Aloise et al benchmark instances.

## I. Uniform points

Generated directly by the C++ program with the provided SEED_PROBLEM.
The coordinates are generated using `std::uniform_real_distribution<double> distrib(0, 100)` for each coordinate.

## II. Franti et al benchmark instances.

The data has been downloaded from http://cs.uef.fi/sipu/datasets/ (as specified in the paper).
More precisely the datasets A, S, G2, Birch, DIM and Unbalanced have been downloaded in their txt format (cf folder data/src_datasets/clustering/benchmark_franti).

The file `num_clusters.json` contains the number of clusters for each dataset. It has been manually created.

Then the script create_datasets.py has been used to create the hdf5 file containing the datasets (points coordinates, number of dimensions and number of clusters for each instance).

## III. Aloise et al benchmark instances.

The source files are provided in the folder data/src_datasets/clustering/benchmark_aloise

The script FIBI.get_datasets.clustering.create_datasets.main_aloise has been used to create the hdf5 file containing the datasets (points coordinates, number of dimensions and number of clusters for each instance).

A metadata file has been created to store:

- the number of dimensions
- the number of clusters
- the id of each instance in the hdf5 file
"""
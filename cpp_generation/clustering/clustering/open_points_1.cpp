#include "open_points_1.h"
#include <filesystem>
#include <iostream>
//former benchmark_points
double* open_points_1(int i, std::string filename, Config* config, bool load_num_clust) {
    double* points;
    std::string dataset_name = std::to_string(i);
    const H5std_string FILE_NAME_BENCHMARK(filename);
    //Exception::dontPrint();
    H5std_string DATASET_NAME(dataset_name);
    try {
        H5File  file(FILE_NAME_BENCHMARK, H5F_ACC_RDONLY);
        Group group_points(file.openGroup("points"));
        Group group_dims(file.openGroup("dims"));
        Group group_num_clust(file.openGroup("num_clusters"));
        // Dimensions
        DataSet dataset_dims = group_dims.openDataSet(DATASET_NAME);
        DataSpace dataspace_dims = dataset_dims.getSpace();
        hsize_t rank_dims;
        hsize_t dims_dims[1];
        rank_dims = dataspace_dims.getSimpleExtentDims(dims_dims);
        double* dims_val = new double[dims_dims[0]];
        dataset_dims.read(dims_val, PredType::NATIVE_DOUBLE);
        config->NUM_DIM = (int)dims_val[0];
        delete[] dims_val;
        // Points
        DataSet dataset_points = group_points.openDataSet(DATASET_NAME);
        DataSpace dataspace_points = dataset_points.getSpace();
        hsize_t dims_points[1];
        hsize_t rank_points;
        rank_points = dataspace_points.getSimpleExtentDims(dims_points);
        points = new double[dims_points[0]];
        dataset_points.read(points, PredType::NATIVE_DOUBLE);
        config->NUM_POINTS = dims_points[0] / config->NUM_DIM;
        if (load_num_clust) {
            // Num of clusters
            DataSet dataset_num_clust = group_num_clust.openDataSet(DATASET_NAME);
            DataSpace dataspace_num_clust = dataset_num_clust.getSpace();
            hsize_t dims_num_clust[1];
            hsize_t rank_num_clust;
            rank_num_clust = dataspace_num_clust.getSimpleExtentDims(dims_num_clust);
            double* num_clust = new double[dims_num_clust[0]];
            dataset_num_clust.read(num_clust, PredType::NATIVE_DOUBLE);
            config->NUM_CLUST = (int)num_clust[0];
            delete[] num_clust;
        }
    }
    catch (FileIException error) {
        error.printErrorStack();
        exit(-1);
    }
    catch (DataSetIException error) {
        error.printErrorStack();
        exit(-1);
    }
    catch (std::exception& e) {
        exit(-1);
    }
    return points;
}
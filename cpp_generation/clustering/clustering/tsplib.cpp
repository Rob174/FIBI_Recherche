#include "tsplib.h"

double* tsplib_sample_points(Config* config) {
    std::mt19937 gen_dist(config->SEED_POINTS);
    try {
        Exception::dontPrint();
        H5File  file(FILE_NAME, H5F_ACC_RDWR);
        // thx to https://stackoverflow.com/questions/32000386/hdf5-c-i-need-get-group-list-dataset-list-attribute-list-in-hdf5-file
        const hsize_t num_keys = file.getNumObjs();
        std::string dataset_name = file.getObjnameByIdx(config->SEED_POINTS);
        return tsplib_points(dataset_name, config);
    }
    catch (FileIException error) {
        error.printErrorStack();
        exit(-1);
    }
    catch (DataSetIException error) {
        error.printErrorStack();
        exit(-1);
    }
}
double* tsplib_points(std::string dataset_name, Config* config) {
    double* points;
    try {
        Exception::dontPrint();
        H5std_string DATASET_NAME(dataset_name);
        H5File  file(FILE_NAME, H5F_ACC_RDWR);
        DataSet dataset = file.openDataSet(DATASET_NAME);
        DataSpace dataspace = dataset.getSpace();
        hsize_t rank;
        hsize_t dims[1];
        rank = dataspace.getSimpleExtentDims(dims);
        points = new double[dims[0]];
        dataset.read(points, PredType::NATIVE_DOUBLE);
        config->NUM_POINTS = dims[0] / config->NUM_DIM;
    }
    catch (FileIException error) {
        error.printErrorStack();
        exit(-1);
    }
    catch (DataSetIException error) {
        error.printErrorStack();
        exit(-1);
    }
    return points;
}
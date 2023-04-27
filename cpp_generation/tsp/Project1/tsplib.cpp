#include "tsplib.h"

double* tsplib_sample_points(const H5std_string FILE_NAME,Config*config) {
    try {
        Exception::dontPrint();
        FILE* fileObj;
        errno_t err;
        if ((err = fopen_s(&fileObj,FILE_NAME.c_str(), "r")) != 0) {
            std::cout << "Dataset does not exists" << std::endl;
            exit(1);
        }
        else {
            fclose(fileObj);
        }
        H5File  file(FILE_NAME, H5F_ACC_RDWR);
        // thx to https://stackoverflow.com/questions/32000386/hdf5-c-i-need-get-group-list-dataset-list-attribute-list-in-hdf5-file
        const hsize_t num_keys = file.getNumObjs();
        std::ostringstream os;
        os << config->SEED_POINTS;
        std::string dataset_name = os.str();
        return tsplib_points(dataset_name, FILE_NAME,config);
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
double* tsplib_points(std::string dataset_name, const H5std_string FILE_NAME,Config*config) {
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
#include "save_to_hdf5.h"

void save_to_hdf5(Result* res, Config* conf) {
    const H5std_string FILE_NAME_OUT(conf->out_dataset);
    try {
        Exception::dontPrint();

        if (!std::filesystem::exists(FILE_NAME_OUT)) {
            std::cout << "file does not exist" << std::endl;
            exit(1);
        }
        std::string identifier;
        std::ostringstream os;
        int IMPR_CLASS;
        int IT_ORDER;
        int INIT_CHOICE;
        int TOUR_ALGO;
        os << res->get_config()->SEED_GLOB << "," 
            << res->get_config()->NUM_POINTS << ","
            << res->get_config()->DATASET << ","
            << res->get_config()->NUM_CLUST << ","
            << res->get_config()->SEED_POINTS << ","
            << res->get_config()->TOUR_ALGO << ","
            << res->get_config()->SEED_ASSIGN << ","
            << res->get_config()->IT_ORDER << ","
            << res->get_config()->IMPR_CLASS;
        identifier = os.str();

        H5File file(FILE_NAME_OUT, H5F_ACC_RDWR);
        // Save input output of algorithm
        // 1. Metadata
        Group group1(file.openGroup("metadata"));
        std::vector<double>* results = res->get_result();
        double* data = new double[results->size()];
        for (std::size_t i = 0; i < results->size(); ++i) {
            data[i] = (*results)[i];
        }
        const int RANK1 = 1;
        hsize_t dims1[RANK1];
        dims1[0] = results->size();
        DataSpace dataspace1(RANK1, dims1);
        DataSet dataset1 = group1.createDataSet(identifier, PredType::NATIVE_DOUBLE, dataspace1);
        dataset1.write(data, PredType::NATIVE_DOUBLE);
        delete results;

    }
    catch (FileIException error) {
        error.printErrorStack();
        std::cout << "Error writing dataset: H5File operations" << std::endl;
        exit(-1);
    }
    catch (DataSetIException error) {
        error.printErrorStack();
        std::cout << "Error writing dataset: DataSet operations" << std::endl;
        exit(-1);
    }
    catch (DataSpaceIException error) {
        error.printErrorStack();
        std::cout << "Error writing dataset: DataSpace operations" << std::endl;
        exit(-1);
    }
}

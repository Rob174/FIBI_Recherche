#ifndef DATASET_H
#define DATASET_H
#include "result.h"
#include <string>
#include <list>
#include <iostream>
#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <stdlib.h>
#include "H5Cpp.h"
#include <filesystem>
using namespace H5;

// #define DUMP_CLUSTERS
template <typename ResultType>
void create_dataset(ResultType* res, bool clean = false)
{
    const H5std_string FILE_NAME("dataset.hdf5");
    // Turn off the auto-printing when failure occurs so that we can handle the errors appropriately
    Exception::dontPrint();
    try
    {
        // Create a new file using the default property lists. check if exists and if not create
        bool file_exists = std::filesystem::exists(FILE_NAME);
        if (!file_exists)
        {
            std::cout << "file does not exist, creating it..." << std::endl;
        }
        else if (clean)
        {
            std::cout << "Removing existing dataset..." << std::endl;
            system("del .\\dataset.hdf5");
        }
        H5File file(FILE_NAME, !file_exists || file_exists && clean ? H5F_ACC_TRUNC : H5F_ACC_RDWR);
        std::string identifier = res->key();

        std::vector<std::string> gpes = res->group();
        std::vector<std::vector<double>> data = res->data();
        if (gpes.size() != data.size())
        {
            THROW_EXC5("Different number of groups and available data with ", std::to_string(gpes.size()), "groups and ", std::to_string(data.size()), " data", 6);
        }
        // Save input output of algorithm iterating over the elements to save (ex: metadata, intermediate solutions)
        const int RANK = 1;
        for (int i = 0; i < gpes.size(); i++)
        {
            const std::string gpe = gpes.at(i);
            const std::vector<double> d = data.at(i);
            Group group;
            if (file.nameExists(gpe))
            {
                group = file.openGroup(gpe);
            }
            else
            {
                std::cout << "Creating group " << gpe << std::endl;
                group = file.createGroup(gpe);
            }
            hsize_t dims[RANK];
            dims[0] = d.size();
            DataSpace dataspace(RANK, dims);
            if (group.nameExists(identifier))
            {
                THROW_EXC3("Dataset ", identifier, " already exists.", 7);
            }
            DataSet dataset = group.createDataSet(identifier, PredType::NATIVE_DOUBLE, dataspace);
            double* res = new double[dims[0]];
            for (std::size_t j = 0; j < dims[0]; j++)
            {
                res[j] = d.at(j);
            }
            dataset.write(res, PredType::NATIVE_DOUBLE);
            delete[] res;
        }
    }
    catch (FileIException error)
    {
        error.printErrorStack();
        THROW_EXC1("Error writing dataset: H5File operations", 500);
    }
    catch (DataSetIException error)
    {
        error.printErrorStack();
        THROW_EXC1("Error writing dataset: DataSet operations", 501);
    }
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
        THROW_EXC1("Error writing dataset: DataSpace operations", 502);
    }
    catch (GenericException e) {
        THROW_EXC1(e.what(), 7);
    }
    catch (const std::exception& exc)
    {
        THROW_EXC2("Other exception ", exc.what(), 503);
    }
}
;
#endif
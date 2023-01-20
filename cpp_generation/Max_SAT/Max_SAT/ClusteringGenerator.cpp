#include "ClusteringGenerator.h"

double *uniform_points(const int num_points, const int num_dims, const int seed, const int minV, const int maxV)
{
    std::mt19937 gen_points(seed);
    std::uniform_real_distribution<> dis(minV, maxV);
    double *points = new double[num_points * num_dims];
    for (int i = 0; i < num_points * num_dims; i++)
    {
        points[i] = dis(gen_points);
    }
    return points;
}
double* test_points() {
    double points[] = {
        78.383235,  31.307694,
        63.483371,  93.723736,
        24.904309,   4.286545,
        75.807586,  44.086720
    };
    
    return copy_array<double, double>(points, 8);
}
int* test_clusters() {
    int clusters[] = {
        1,1,0,0
    };
    return copy_array<int, int>(clusters, 4);
}
double* open_points_1(const int i, std::string filename, ClusteringConfig* conf, bool load_num_clust) {
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
        conf->update_keys(std::map<std::string,int>{{stringify(NUM_DIM), (int)dims_val[0]}});
        delete[] dims_val;
        // Points
        DataSet dataset_points = group_points.openDataSet(DATASET_NAME);
        DataSpace dataspace_points = dataset_points.getSpace();
        hsize_t dims_points[1];
        hsize_t rank_points;
        rank_points = dataspace_points.getSimpleExtentDims(dims_points);
        points = new double[dims_points[0]];
        dataset_points.read(points, PredType::NATIVE_DOUBLE);
        conf->update_keys(std::map<std::string,int>{{stringify(NUM_POINTS), dims_points[0] / M_cst_frc(NUM_DIM)}});
        if (load_num_clust) {
            // Num of clusters
            DataSet dataset_num_clust = group_num_clust.openDataSet(DATASET_NAME);
            DataSpace dataspace_num_clust = dataset_num_clust.getSpace();
            hsize_t dims_num_clust[1];
            hsize_t rank_num_clust;
            rank_num_clust = dataspace_num_clust.getSimpleExtentDims(dims_num_clust);
            double* num_clust = new double[dims_num_clust[0]];
            dataset_num_clust.read(num_clust, PredType::NATIVE_DOUBLE);
            conf->update_keys(std::map<std::string,int>{{stringify(NUM_CLUST),(int)num_clust[0]}});
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

int* random_clust(ClusteringConfig* conf) {
    int * clusters = new int[M_cst_frc(NUM_POINTS)];
    std::mt19937 gen_assign(M_cst_frc(SEED_ASSIGN));
    std::uniform_int_distribution<> distrib(0, M_cst_frc(NUM_CLUST) - 1);
    for (int i = 0; i < M_cst_frc(NUM_POINTS); i++) {
        clusters[i] = distrib(gen_assign);
    }
    return clusters;
}

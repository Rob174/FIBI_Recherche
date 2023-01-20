#include "TSPGenerator.h"

double *open_file(std::string filename, const int instance_idx)
{
    double *points;
    std::string dataset_name = std::to_string(instance_idx);
    const H5std_string FILE_NAME(filename);
    H5std_string DATASET_NAME(dataset_name);

    try
    {
        Exception::dontPrint();
        H5std_string DATASET_NAME(dataset_name);
        H5File file(FILE_NAME, H5F_ACC_RDWR);
        DataSet dataset = file.openDataSet(DATASET_NAME);
        DataSpace dataspace = dataset.getSpace();
        hsize_t rank;
        hsize_t dims[1];
        rank = dataspace.getSimpleExtentDims(dims);
        points = new double[dims[0]];
        dataset.read(points, PredType::NATIVE_DOUBLE);
    }
    catch (FileIException error)
    {
        error.printErrorStack();
        exit(-1);
    }
    catch (DataSetIException error)
    {
        error.printErrorStack();
        exit(-1);
    }
    catch (std::exception &e)
    {
        exit(-1);
    }
    return points;
}
int *random_tour(const int num_towns, const int seed)
{
    std::mt19937 gen_dist(seed);
    int *tour = new int[num_towns];
    for (int i = 0; i < num_towns; i++)
        tour[i] = i;
    for (int i = 0; i < num_towns - 2; i++)
    {
        std::uniform_int_distribution<> dis_choice(i, num_towns - 1);
        int r = dis_choice(gen_dist);
        int tmp = tour[i];
        tour[i] = tour[r];
        tour[r] = tmp;
    }
    return tour;
}

int *improved_tour(const int num_towns, const int seed, DistanceMatrix *distances)
{
    int *tour = new int[num_towns];
    // Add a set to avoid duplicates (a points can be the nearest one for two different points)
    std::set<int> available_points;
    for (int i = 0; i < num_towns; i++)
    {
        available_points.insert(i);
    }
    std::mt19937 gen_dist(seed);
    std::uniform_int_distribution<> dis_choice(0, num_towns - 1);
    int start_point = dis_choice(gen_dist);
    tour[0] = start_point;
    available_points.erase(start_point);
    for (int i = 0; i < num_towns - 1; i++)
    {
        // Find the argmin of distance
        double min = std::numeric_limits<double>::max();
        int argmin = -1;
        for (int j : available_points)
        {
            double d = distances->get(tour[i], j);
            if (d < min)
            {
                min = d;
                argmin = j;
            }
        }
        tour[i + 1] = argmin;
        // Erase the point to avoid duplicates
        available_points.erase(argmin);
    }
    return tour;
}
int *improved_rand_tour(const int num_towns, const int seed, DistanceMatrix *distances)
{
    const int N = 4;
    int *tour = new int[num_towns];
    // Add a set to avoid duplicates (a points can be the nearest one for two different points)
    std::set<int> available_points;
    for (int i = 0; i < num_towns; i++)
    {
        available_points.insert(i);
    }
    std::mt19937 gen_dist(seed);
    std::uniform_int_distribution<> dis_choice(0, num_towns - 1);
    int start_point = dis_choice(gen_dist);
    tour[0] = start_point;
    available_points.erase(start_point);
    for (int i = 0; i < num_towns - 1; i++)
    {
        // Prepare for the top N
        std::vector<std::pair<double, int>> top_x_data;
        for (int j : available_points)
        {
            double d = distances->get(tour[i], j);
            top_x_data.push_back(std::make_pair(d, j));
        }
        // Take top N
        const int size_top = std::min(N, (int)(top_x_data.size()));
        std::nth_element(top_x_data.begin(), top_x_data.begin() + size_top, top_x_data.end(),
                         [](auto &l, auto &r)
                         { return l.first < r.first; });
        std::uniform_int_distribution<> dis_top(0, size_top - 1);
        const int selected_of_top_id = dis_top(gen_dist);
        tour[i + 1] = top_x_data.at(selected_of_top_id).second;
        // Erase the point to avoid duplicates
        available_points.erase(top_x_data.at(selected_of_top_id).second);
    }
    return tour;
}

double *test_towns()
{
    double p[] = {
        10,0,
        0, 4,
        14, 5,
        6, 6,
        1, 12,
        18, 12,
        3, 16
    };
    return copy_array<double,double>(p, 14);
}
int* test_tour() {
    return new int[]{0,5,2,6,3,4,1};
}
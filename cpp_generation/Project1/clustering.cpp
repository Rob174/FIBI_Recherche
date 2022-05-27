#include "clustering.h"
#include "H5Cpp.h"
#include "utils.h"

#define GRID_COORD_MIN 0.
#define GRID_COORD_MAX 100.
std::tuple<int, char**> random_argv(int loop_id, int init) {
    int num_points = 1000;
    const int clusters[] = { 2,4,8,16 };
    int num_clust = clusters[0];
    int it_order = 0;
    int impr_choice = loop_id % 2;
    std::uniform_int_distribution<> distrib(0, 1);
    std::mt19937 gen_assign(long(loop_id / 2));
    int init_choice = distrib(gen_assign);//init;

    int argc = 7;
    std::string
        s_num_pt = std::to_string(num_points),
        s_init = std::to_string(init_choice),
        s_num_clust = std::to_string(num_clust),
        s_it_order = std::to_string(it_order),
        s_impr_choice = std::to_string(impr_choice);
    std::vector<std::string> v;
    v.push_back("prog_name");
    v.push_back(s_num_pt.c_str());
    v.push_back("2");
    v.push_back(s_num_clust.c_str());
    v.push_back(s_it_order.c_str());
    v.push_back(s_impr_choice.c_str());
    v.push_back(s_init.c_str());
    char** argv = new char*[v.size()];
    int index = 0;
    for (auto& a : v) {
        argv[index] = new char[a.length()+1];
        strncpy_s(argv[index], a.length() + 1, a.c_str(), a.length() + 1);
        index++;
    }
    return std::make_tuple(argc, argv);
}
std::tuple<Config*, IterationOrder*, ImprovementChoice*, Initializer* ,Result*> get_config(int argc, char** argv, long seed_points,long seed_assign)
{
    Config* config = new Config;
    if (argc != 7)
    {
        printf("4 arguments expected, found %d\n", argc - 1);
        printf("Expected arguments: NUM_POINTS NUM_DIM NUM_CLUST ITORDER IMPRCHOICE INITCHOICE\n");
        IterationOrderFactory::print_doc();
        ImprFactory::print_doc();
        InitializerFactory::print_doc();
        exit(1);
    }
    config->NUM_POINTS = atoi(argv[1]);
    if (config->NUM_POINTS <= 1) {
        exit(1);
    }
    config->NUM_DIM = atoi(argv[2]);
    if (config->NUM_DIM != 2) {
        exit(1);
    }
    config->NUM_CLUST = atoi(argv[3]);
    if (config->NUM_CLUST == 0 || config->NUM_CLUST == 1 || config->NUM_CLUST >= config->NUM_POINTS) {
        std::cout << "Wrong number of clusters related to num of points with " << config->NUM_POINTS << "points and " << config->NUM_CLUST << " clusters" << std::endl;
        exit(1);
    }
    config->IT_ORDER = atoi(argv[4]);
    if (config->IT_ORDER > 2) {
        exit(1);
    }
    config->IMPR_CLASS = atoi(argv[5]);
    if (config->IMPR_CLASS > 10) {
        exit(1);
    }
    config->INIT_CHOICE = atoi(argv[6]);
    if (config->INIT_CHOICE > 10) {
        exit(1);
    }
    config->SEED_POINTS = seed_points;
    config->SEED_ASSIGN = seed_assign;
    IterationOrder* iteration_order = IterationOrderFactory::create(config, config->IT_ORDER);
    Result* result = new Result(config);
    ImprovementChoice* impr = ImprFactory::create(config->IMPR_CLASS, result, config);
    Initializer* initializer = InitializerFactory::create(config->INIT_CHOICE);
    return std::make_tuple(config, iteration_order,impr, initializer,result);
}
void clean(Config* config, Clustering*clust, Clustering* initial_clustering, IterationOrder* iteration_order, Result* result, ImprovementChoice* impr, Initializer* initializer) {
    delete clust->p_c;
    delete clust->n_p_p_c;
    delete clust->c_a;
    delete clust->c_c;
    delete initial_clustering->p_c;
    delete initial_clustering->n_p_p_c;
    delete initial_clustering->c_a;
    delete initial_clustering->c_c;
    delete clust;
    delete initial_clustering;
    delete config;
    delete iteration_order;
    delete result;
    delete impr;
    delete initializer;
}
void random_initialize_point(Clustering* clustering, Config* config) {
    std::mt19937 gen_points(config->SEED_POINTS);
    std::uniform_real_distribution<> dis(GRID_COORD_MIN, GRID_COORD_MAX);
    for (int i = 0; i < config->NUM_POINTS * config->NUM_DIM; i++)
        clustering->p_c[i] = dis(gen_points);
}
void initialize_moons(Clustering* clustering, Config* config) {
    const float std_min = 0.01;
    const float std_max = 0.1;
    const double pi = 3.14159265358979323846;
    const double step = pi / double(config->NUM_POINTS / config->NUM_CLUST);
    std::mt19937 gen_noise(config->SEED_POINTS);
    std::uniform_real_distribution<double> dis_std(std_min, std_max);
    const double std_noise = dis_std(gen_noise);
    const double scale_factor = (double)(GRID_COORD_MAX - GRID_COORD_MIN)/(double)(2.);
    std::normal_distribution<double> dis_noise(0, std_noise);
    std::uniform_real_distribution<double> dis_theta(0, 2*pi);
    double theta = dis_theta(gen_noise);
    double rotation[2][2] = {
        {cos(theta), -sin(theta)},
        {sin(theta),cos(theta)}
    };
    for (int i = 0; i < config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST; i++) {
        if (i % 2 == 0) {
            clustering->p_c[i] = (cos(step * (i / 2))+1)/3.0;
        }
        else {
            clustering->p_c[i] = (sin(step * (i / 2)) + 0.5) / 1.5;
        }

        clustering->p_c[i] += dis_noise(gen_noise);
        if (i % 2 == 1) {
            clustering->p_c[i] -= 0.5;
            //manual matrixm multiplication
            double new_x = rotation[0][0] * clustering->p_c[i - 1] + rotation[0][1] * clustering->p_c[i];
            double new_y = rotation[1][0] * clustering->p_c[i - 1] + rotation[1][1] * clustering->p_c[i];
            clustering->p_c[i - 1] = new_x+0.5;
            clustering->p_c[i] = new_y+0.5;
            clustering->p_c[i-1] = clustering->p_c[i-1]*scale_factor+ GRID_COORD_MIN;
            clustering->p_c[i] = clustering->p_c[i] * scale_factor + GRID_COORD_MIN;
        }
    }
    const int offset_pt = config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST;
    for (int i = 0; i < config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST; i++) {
        if (i % 2 == 0) {
            clustering->p_c[offset_pt + i] = (1 - cos(step * (i / 2)) + 1) / 3.0;
        }
        else {
            clustering->p_c[offset_pt + i] = (1 - sin(step * (i / 2)) - 0.5+0.5)/1.5;
        }
        clustering->p_c[offset_pt + i] += dis_noise(gen_noise);
        if (i % 2 == 1) {
            clustering->p_c[offset_pt + i] -= 0.5;
            //manual matrix multiplication
            double new_x = rotation[0][0] * clustering->p_c[offset_pt + i - 1] + rotation[0][1] * clustering->p_c[offset_pt + i];
            double new_y = rotation[1][0] * clustering->p_c[offset_pt + i - 1] + rotation[1][1] * clustering->p_c[offset_pt + i];
            clustering->p_c[offset_pt + i - 1] = new_x + 0.5;
            clustering->p_c[offset_pt + i] = new_y + 0.5;
            clustering->p_c[offset_pt + i - 1] = clustering->p_c[offset_pt + i - 1] * scale_factor + GRID_COORD_MIN;
            clustering->p_c[offset_pt + i] = clustering->p_c[offset_pt + i] * scale_factor + GRID_COORD_MIN;
        }
    }
}
void initialize_blobs(Clustering* clustering, Config* config) {
    const float std_min = 0.01;
    const float std_max = 0.1;
    const float scale_factor = GRID_COORD_MAX - GRID_COORD_MIN;
    const float dist_thresh = float(GRID_COORD_MAX - GRID_COORD_MIN) / 10.;
    std::mt19937 gen_points(config->SEED_POINTS);
    std::uniform_real_distribution<double> dis_std(std_min, std_max);
    double centers[2][2]; 
    double stds[2];
    std::uniform_real_distribution<double> dis_centers(GRID_COORD_MIN, GRID_COORD_MAX);
    for (int i_cent = 0; i_cent < config->NUM_DIM; i_cent++) {
        centers[0][i_cent] = dis_centers(gen_points);
    }
    for (int i = 0; i < 2; i++) {
        stds[i] = dis_std(gen_points) * scale_factor;
    }
    double distance = 0;
    int counter = 1000;
    while (distance < dist_thresh  && counter > 0) {
        for (int i_cent = 0; i_cent < config->NUM_DIM; i_cent++) {
            centers[1][i_cent] = dis_centers(gen_points);
        }
        distance = sqrt(dist(centers[0], centers[1],config))- 2*stds[0] -2*stds[1];
        counter--;
    }
    for (int i_clust = 0; i_clust < config->NUM_CLUST; i_clust++) {
        double* center = centers[i_clust];
        std::normal_distribution<double> dis_norm(0, 1);
        // We will shift the points afterwards 
        /*
        if X ~ N(0,1) ; with Y=aX+b -> Y ~ N(a x 0 + b, a^2 x 1^2)
        We want N(mu,sigma^2)
        -> b=mu
        -> a=sigma
        */
        const int offset_pt = i_clust * config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST;
        for (int i_pt = 0; i_pt < config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST; i_pt++) {// equal number of points for each cluster
            clustering->p_c[offset_pt + i_pt] = dis_norm(gen_points) * stds[i_clust] + center[i_pt % 2];
        }
    }
}
void initialize(Clustering* clustering, Config* config)
{
    // Initialize points coordinates
    clustering->p_c = new double[config->NUM_POINTS * config->NUM_DIM];
    initialize_moons(clustering, config);
    // Initialize number of points per cluster
    clustering->n_p_p_c = new int[config->NUM_CLUST];
    for (int i = 0; i < config->NUM_CLUST; i++)
        clustering->n_p_p_c[i] = 0;
    // Initialize cluster intial assignements
    clustering->c_a = new int[config->NUM_POINTS];
    std::mt19937 gen_assign(config->SEED_ASSIGN);
    std::uniform_int_distribution<> distrib(0, config->NUM_CLUST - 1);
    for (int i = 0; i < config->NUM_POINTS; i++)
        clustering->c_a[i] = distrib(gen_assign);
    // Initialize cluster centroids
    clustering->c_c = new double[config->NUM_CLUST * config->NUM_DIM];
    for (int i = 0; i < config->NUM_CLUST * config->NUM_DIM; i++)
        clustering->c_c[i] = 0.;
    // Compute cluster centroids
    for (int i = 0; i < config->NUM_POINTS; i++)
    {
        int cluster_id = clustering->c_a[i];
        for (int j = 0; j < config->NUM_DIM; j++)
        {
            clustering->c_c[cluster_id * config->NUM_DIM + j] += clustering->p_c[i * config->NUM_DIM + j];
        }
        clustering->n_p_p_c[cluster_id]++;
    }
    for (int i = 0; i < config->NUM_CLUST; i++)
    {
        for (int j = 0; j < config->NUM_DIM; j++)
        {
            clustering->c_c[i * config->NUM_DIM + j] = clustering->n_p_p_c[i] > 0 ? clustering->c_c[i * config->NUM_DIM + j]/clustering->n_p_p_c[i] : 0.;
        }
    }
}
Clustering* deepcopy(Clustering* clust,Config* config) {
    Clustering* new_clustering = new Clustering();
    // Initialize points coordinates
    new_clustering->p_c = new double[config->NUM_POINTS * config->NUM_DIM];
    for (int i = 0; i < config->NUM_POINTS * config->NUM_DIM; i++)
        new_clustering->p_c[i] = clust->p_c[i];
    // Initialize number of points per cluster
    new_clustering->n_p_p_c = new int[config->NUM_CLUST];
    for (int i = 0; i < config->NUM_CLUST; i++)
        new_clustering->n_p_p_c[i] = clust->n_p_p_c[i];
    // Initialize cluster intial assignements
    new_clustering->c_a = new int[config->NUM_POINTS];
    for (int i = 0; i < config->NUM_POINTS; i++)
        new_clustering->c_a[i] = clust->c_a[i];
    // Initialize cluster centroids
    new_clustering->c_c = new double[config->NUM_CLUST * config->NUM_DIM];
    for (int i = 0; i < config->NUM_CLUST * config->NUM_DIM; i++)
        new_clustering->c_c[i] = clust->c_c[i];
    return new_clustering;
}
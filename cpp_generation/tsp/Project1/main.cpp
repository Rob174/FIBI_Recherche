#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "tests.h"
#include "factory.h"
#include <sstream>
#define DEBUG
void reproduce_table1_publi(int start, int stop,int index) {
    using namespace std;
    Config conf;
    conf.NUM_DIM = 2;
    const int num_algo = 4, num_it_order = 3;
    const int tot_num_pts = 26;
    const int num_pts[tot_num_pts] = { 20,30,40,50,60,70,80,90,100,110,120,130,140,150,200,250,300,350,400,450,500,600,700,800,900,1000 };
    //Table 1 experiments : uniform distribution
    conf.DATASET = 0;
    conf.TOUR_ALGO = 0;
    conf.IT_ORDER = 1;
    int seed = 0;
    int seedGlob = 0;
    for (int i_pt = 0; i_pt < tot_num_pts; i_pt++) {
        conf.NUM_POINTS = num_pts[i_pt];
        for (int algo = 0; algo < num_algo; algo++) {
            conf.IMPR_CLASS = algo;
            int num_rep = conf.NUM_POINTS <= 150 ? 1000 : 100;
            seed = 0;
            for (int rep = 0; rep < num_rep; rep++) {
                conf.SEED_ASSIGN = seed;
                conf.SEED_POINTS = seed;

                std::ostringstream oss;
                if(index == -1)
                    oss << "dataset.hdf5";
                conf.out_dataset = oss.str();
                if (seedGlob <= stop && seedGlob >= start) {
                    run_algorithm(&conf);
#if defined(DEBUG)
                    conf.print();
#endif
                }
                seed++;
                seedGlob++;
            }
        }
    }
    std::cout << seedGlob << std::endl;
}
void reproduce_table2_publi(int start, int stop, int index) {
    using namespace std;
    Config conf;
    conf.NUM_DIM = 2;
    const int num_algo = 4, num_it_order = 3, num_rep = 10, num_instances = 23;
    //Table 2 experiments : tsplib
    conf.DATASET = 2;
    conf.TOUR_ALGO = 0;
    conf.IT_ORDER = 1;
    int seed = 0;
    int seedGlob = 0;
    for (int instance = 0; instance < num_instances; instance++) {
        conf.SEED_POINTS = instance;
        for (int algo = 0; algo < num_algo; algo++) {
            conf.IMPR_CLASS = algo;
            seed = 0;
            for (int rep = 0; rep < num_rep; rep++) {
                conf.SEED_ASSIGN = seed;

                std::ostringstream oss;
                if (index == -1)
                    oss << "dataset.hdf5";
                conf.out_dataset = oss.str();
                if (seedGlob <= stop && seedGlob >= start) {
                    run_algorithm(&conf);
#if defined(DEBUG)
                    conf.print();
#endif
                }
                seed++;
                seedGlob++;
            }
        }
    }
    std::cout << seedGlob << std::endl;
}
void reproduce_table3_publi(int start, int stop, int index) {
    using namespace std;
    Config conf;
    conf.NUM_DIM = 2;
    const int num_algo = 4, num_it_order = 3;
    const int tot_num_pts = 26;
    const int num_pts[tot_num_pts] = { 20,30,40,50,60,70,80,90,100,110,120,130,140,150,200,250,300,350,400,450,500,600,700,800,900,1000 };
    //Table 1 experiments : uniform distribution
    conf.DATASET = 3;
    conf.TOUR_ALGO = 0;
    conf.IT_ORDER = 1;
    int seed = 0;
    int seedGlob = 0;
    for (int i_pt = 0; i_pt < tot_num_pts; i_pt++) {
        conf.NUM_POINTS = num_pts[i_pt];
        for (int algo = 0; algo < num_algo; algo++) {
            conf.IMPR_CLASS = algo;
            int num_rep = conf.NUM_POINTS <= 150 ? 1000 : 100;
            seed = 0;
            for (int rep = 0; rep < num_rep; rep++) {
                conf.SEED_ASSIGN = seed;
                conf.SEED_POINTS = seed;

                std::ostringstream oss;
                if (index == -1)
                    oss << "dataset.hdf5";
                conf.out_dataset = oss.str();
                if (seedGlob <= stop && seedGlob >= start) {
                    run_algorithm(&conf);
#if defined(DEBUG)
                    conf.print();
#endif
                }
                seed++;
                seedGlob++;
            }
        }
    }
}
void reproduce_table1_impr(int start, int stop, int index) {
    using namespace std;
    Config conf;
    conf.NUM_DIM = 2;
    const int num_algo = 4, num_it_order = 3;
    const int tot_num_pts = 26;
    const int num_pts[tot_num_pts] = { 20,30,40,50,60,70,80,90,100,110,120,130,140,150,200,250,300,350,400,450,500,600,700,800,900,1000 };
    //Table 1 experiments : uniform distribution
    conf.DATASET = 0;
    conf.TOUR_ALGO = 1;
    conf.IT_ORDER = 1;
    int seed = 0;
    int seedGlob = 0;
    for (int i_pt = 0; i_pt < tot_num_pts; i_pt++) {
        conf.NUM_POINTS = num_pts[i_pt];
        for (int algo = 0; algo < num_algo; algo++) {
            conf.IMPR_CLASS = algo;
            int num_rep = conf.NUM_POINTS <= 150 ? 1000 : 100;
            seed = 0;
            for (int rep = 0; rep < num_rep; rep++) {
                conf.SEED_ASSIGN = seed;
                conf.SEED_POINTS = seed;

                std::ostringstream oss;
                if (index == -1)
                    oss << "dataset.hdf5";
                conf.out_dataset = oss.str();
                if (seedGlob <= stop && seedGlob >= start) {
                    run_algorithm(&conf);
#if defined(DEBUG)
                    conf.print();
#endif
                }
                seed++;
                seedGlob++;
            }
        }
    }
    std::cout << seedGlob << std::endl;
}
void reproduce_table2_impr(int start, int stop, int index) {
    using namespace std;
    Config conf;
    conf.NUM_DIM = 2;
    const int num_algo = 4, num_it_order = 3, num_rep = 10, num_instances = 23;
    //Table 2 experiments : tsplib
    conf.DATASET = 2;
    conf.TOUR_ALGO = 1;
    conf.IT_ORDER = 1;
    int seed = 0;
    int seedGlob = 0;
    for (int instance = 0; instance < num_instances; instance++) {
        conf.SEED_POINTS = instance;
        for (int algo = 0; algo < num_algo; algo++) {
            conf.IMPR_CLASS = algo;
            seed = 0;
            for (int rep = 0; rep < num_rep; rep++) {
                conf.SEED_ASSIGN = seed;

                std::ostringstream oss;
                if (index == -1)
                    oss << "dataset.hdf5";
                conf.out_dataset = oss.str();
                if (seedGlob <= stop && seedGlob >= start) {
                    run_algorithm(&conf);
#if defined(DEBUG)
                    conf.print();
#endif
                }
                seed++;
                seedGlob++;
            }
        }
    }
    std::cout << seedGlob << std::endl;
}
void reproduce_table3_impr(int start, int stop, int index) {
    using namespace std;
    Config conf;
    conf.NUM_DIM = 2;
    const int num_algo = 4, num_it_order = 3;
    const int tot_num_pts = 26;
    const int num_pts[tot_num_pts] = { 20,30,40,50,60,70,80,90,100,110,120,130,140,150,200,250,300,350,400,450,500,600,700,800,900,1000 };
    //Table 1 experiments : uniform distribution
    conf.DATASET = 3;
    conf.TOUR_ALGO = 1;
    conf.IT_ORDER = 1;
    int seed = 0;
    int seedGlob = 0;
    for (int i_pt = 0; i_pt < tot_num_pts; i_pt++) {
        conf.NUM_POINTS = num_pts[i_pt];
        for (int algo = 0; algo < num_algo; algo++) {
            conf.IMPR_CLASS = algo;
            int num_rep = conf.NUM_POINTS <= 150 ? 1000 : 100;
            seed = 0;
            for (int rep = 0; rep < num_rep; rep++) {
                conf.SEED_ASSIGN = seed;
                conf.SEED_POINTS = seed;

                std::ostringstream oss;
                if (index == -1)
                    oss << "dataset.hdf5";
                conf.out_dataset = oss.str();
                if (seedGlob <= stop && seedGlob >= start) {
                    run_algorithm(&conf);
#if defined(DEBUG)
                    conf.print();
#endif
                }
                seed++;
                seedGlob++;
            }
        }
    }
    std::cout << seedGlob << std::endl;
}
int main(int argc, char *argv[])
{   
    int start = 0;
    int stop = 60801;
    int index = -1;

    if (argc == 3) {
        start = std::stoi(argv[1]);
        stop = std::stoi(argv[2]);
    }
    reproduce_table1_impr(start, stop, index);
    //run_tests();
    return 0;
}
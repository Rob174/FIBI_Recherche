#include "tests.h"
#include "factory.h"
#include "utils.h"
#include <iostream>
#define DEBUG
void reproduce_table1_clust(int start, int stop)
{
    Config conf;
    conf.NUM_DIM = 2;
    conf.POINTS_DISTR = 0;
    conf.IT_ORDER = 1;
    conf.CLUST_IMPR = 0;
    const int
        num_clust_poss = 8,
        num_pts_num_poss = 26,//9
        num_it_order = 3,
        num_algo = 2;
    const int clust[num_clust_poss] = {2, 4, 8, 16, 32, 64, 128, 256};
    const int num_pts[num_pts_num_poss] = { 20,30,40,50,60,70,80,90,100,110,120,130,140,150,200,250,300,350,400,450,500,600,700,800,900,1000 };
    int seed = 0;
    int globSeed = 0;
    for (int i_pt = 0; i_pt < num_pts_num_poss; i_pt++) {
        conf.NUM_POINTS = num_pts[i_pt];
        for(int i_clust = 0; i_clust < num_clust_poss; i_clust++) {
            conf.NUM_CLUST = clust[i_clust];
            for (int algo = 0; algo < num_algo; algo++) {
                conf.IMPR_CLASS = algo;
                int num_rep = 1000;// conf.NUM_POINTS <= 150 ? 1000 : 100;
                seed = 0;
                for (int rep = 0; rep < num_rep; rep++) {
                    conf.SEED_ASSIGN = seed;
                    conf.SEED_POINTS = seed;
                    if (globSeed <= stop && globSeed >= start) {
                        conf.SEED_GLOB = globSeed;
                        if (conf.NUM_POINTS > conf.NUM_CLUST) {
                            run_algorithm(&conf);
                        }
#if defined(DEBUG)
                        std::cout << "SEED_GLOB:" << globSeed;
                        conf.print();
#endif
                    }
                    seed++;
                    globSeed++;
                }
            }
        }
    }
    std::cout << globSeed << std::endl;
    std::cout << stop << "," << start << std::endl;
}
void reproduce_table1_clust_impr(int start, int stop)
{
    Config conf;
    conf.NUM_DIM = 2;
    conf.POINTS_DISTR = 0;
    conf.IT_ORDER = 1;
    conf.CLUST_IMPR = 1;
    const int
        num_clust_poss = 8,
        num_pts_num_poss = 26,//9
        num_it_order = 3,
        num_algo = 2;
    const int clust[num_clust_poss] = { 2, 4, 8, 16, 32, 64, 128, 256 };
    const int num_pts[num_pts_num_poss] = { 20,30,40,50,60,70,80,90,100,110,120,130,140,150,200,250,300,350,400,450,500,600,700,800,900,1000 };
    int seed = 0;
    int globSeed = 0;
    for (int i_pt = 0; i_pt < num_pts_num_poss; i_pt++) {
        conf.NUM_POINTS = num_pts[i_pt];
        for (int i_clust = 0; i_clust < num_clust_poss; i_clust++) {
            conf.NUM_CLUST = clust[i_clust];
            for (int algo = 0; algo < num_algo; algo++) {
                conf.IMPR_CLASS = algo;
                int num_rep = 1000;// conf.NUM_POINTS <= 150 ? 1000 : 100;
                seed = 0;
                for (int rep = 0; rep < num_rep; rep++) {
                    conf.SEED_ASSIGN = seed;
                    conf.SEED_POINTS = seed;
                    if (globSeed <= stop && globSeed >= start) {
                        conf.SEED_GLOB = globSeed;
                        if (conf.NUM_POINTS > conf.NUM_CLUST) {
                            run_algorithm(&conf);
                        }
#if defined(DEBUG)
                        std::cout << "SEED_GLOB:" << globSeed;
                        conf.print();
#endif
                    }
                    seed++;
                    globSeed++;
                }
            }
        }
    }
    std::cout << globSeed << std::endl;
    std::cout << stop << "," << start << std::endl;
}
void reproduce_table2_clust(int start, int stop)
{
    Config conf;
    conf.NUM_DIM = 2;
    conf.POINTS_DISTR = 3;
    conf.IT_ORDER = 1;
    conf.CLUST_IMPR = 0;
    const int
        num_clust_poss = 8,
        num_algo = 2,
        num_instances = 23,
        num_rep = 10;
    const int clust[num_clust_poss] = { 2, 4, 8, 16, 32, 64, 128, 256 };
    int seed = 0;
    int globSeed = 0;
    for (int instance = 0; instance < num_instances; instance++) {
        conf.SEED_POINTS = instance;
        for (int i_clust = 0; i_clust < num_clust_poss; i_clust++) {
            conf.NUM_CLUST = clust[i_clust];
            for (int algo = 0; algo < num_algo; algo++) {
                conf.IMPR_CLASS = algo;
                seed = 0;
                for (int rep = 0; rep < num_rep; rep++) {
                    conf.SEED_ASSIGN = seed;
                    if (globSeed <= stop && globSeed >= start) {
                        run_algorithm(&conf);
#if defined(DEBUG)
                        std::cout << "SEED_GLOB:" << globSeed;
                        conf.print();
#endif
                    }
                    seed++;
                    globSeed++;
                }
            }
        }
    }
    std::cout << globSeed << std::endl;
}
void reproduce_table2_clust_impr(int start, int stop)
{
    Config conf;
    conf.NUM_DIM = 2;
    conf.POINTS_DISTR = 3;
    conf.IT_ORDER = 1;
    conf.CLUST_IMPR = 1;
    const int
        num_clust_poss = 8,
        num_algo = 2,
        num_instances = 23,
        num_rep = 10;
    const int clust[num_clust_poss] = { 2, 4, 8, 16, 32, 64, 128, 256 };
    int seed = 0;
    int globSeed = 0;
    for (int instance = 0; instance < num_instances; instance++) {
        conf.SEED_POINTS = instance;
        for (int i_clust = 0; i_clust < num_clust_poss; i_clust++) {
            conf.NUM_CLUST = clust[i_clust];
            for (int algo = 0; algo < num_algo; algo++) {
                conf.IMPR_CLASS = algo;
                seed = 0;
                for (int rep = 0; rep < num_rep; rep++) {
                    conf.SEED_ASSIGN = seed;
                    if (globSeed <= stop && globSeed >= start) {
                        run_algorithm(&conf);
#if defined(DEBUG)
                        conf.print();
#endif
                    }
                    seed++;
                    globSeed++;
                }
            }
        }
    }
    std::cout << globSeed << std::endl;
}
int main(int argc, char *argv[])
{
    int start = 0;
    int stop = 500000;
    if (argc == 3) {
        start = std::stoi(argv[1]);
        stop = std::stoi(argv[2]);
    }
    reproduce_table1_clust(start, stop);
    // run_tests();
    return 0;
}
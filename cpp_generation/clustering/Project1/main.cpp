#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "tests.h"
#include "factory.h"
#include "utils.h"
void exec_1()
{
    Config conf;
    conf.NUM_DIM = 2;
    const int 
        num_rep = 1000, 
        num_clust_poss = 8, 
        num_pts_num_poss = 18, 
        num_it_order = 3, 
        num_algo = 2, 
        num_init_impr = 2, 
        num_pts_distr = 3;
    const int clust[num_clust_poss] = {2, 4, 8, 16, 32, 64, 128, 256};
    const int num_pts[num_pts_num_poss] = {20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    for (int pts_distrib = 0; pts_distrib < num_pts_distr; pts_distrib++) {
        conf.POINTS_DISTR = pts_distrib;
        for (int algo = 0; algo < num_algo; algo++) {
            conf.IMPR_CLASS = algo;
            for (int init_impr = 0; init_impr < num_init_impr; init_impr++) {
                conf.CLUST_IMPR = init_impr;
                for (int it_order = 0; it_order < num_it_order; it_order++) {
                    conf.IT_ORDER = it_order;
                    if(conf.IMPR_CLASS == 0 && conf.IT_ORDER != 0) {
                        continue;
                    }
                    for (int rep = 0; rep < num_rep; rep++) {
                        for (int clust_poss = 0; clust_poss < num_clust_poss; clust_poss++) {
                            conf.NUM_CLUST = clust[clust_poss];
                            for (int pts_num_poss = 0; pts_num_poss < num_pts_num_poss; pts_num_poss++) {
                                conf.NUM_POINTS = num_pts[pts_num_poss];
                                if(conf.NUM_POINTS > conf.NUM_CLUST) {
                                    run_algorithm(&conf);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
int main(int argc, char *argv[])
{
    exec_1();
    // run_tests();
    return 0;
}
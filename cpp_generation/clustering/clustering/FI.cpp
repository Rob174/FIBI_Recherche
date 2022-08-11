#include "FI.h"
#define EPSILON 1e-5
//#define DEBUG_LV1
//#define DEBUG_LV2
void run_FI(Clustering* clustering, Result* result, IterationOrder* order, Config* config)
{
    double cost = initial_cost(clustering, config);
#if defined(DEBUG_LV2)
    std::cout << "Init clustering" << std::endl;
    std::cout << "Cost:" << cost << std::endl;
    clustering->print_with_ids(config);
#endif
    result->set_init_cost(cost);
    // Local seach
    result->set_time_start();
    bool improvement = true;
    double delta;
    int i_star, j_star, point;
    while (true)
    {
        delta = 0;
        for (int point_moving_id = 0; point_moving_id < config->NUM_POINTS; point_moving_id++)
        {
            int from_cluster_id = clustering->c_a[point_moving_id];
            order->restart(from_cluster_id);
            int to_clust_id = order->next();
            while (to_clust_id != -1) {
                double delta_ij = cost_modif(
                    clustering,
                    from_cluster_id, to_clust_id,
                    &(clustering->p_c[point_moving_id * config->NUM_DIM]), config);
                if (delta_ij < -EPSILON)
                {
                    delta = delta_ij;
                    i_star = from_cluster_id;
                    j_star = to_clust_id;
                    point = point_moving_id;
                    goto outloop;
                }
                result->set_next_iter_glob();
                to_clust_id = order->next();
            }
            order->end_loop();
        }
        if (abs(delta) < EPSILON) {
            result->set_final_cost(cost);
            result->set_final_clust(clustering->p_c, clustering->c_a);
            return;
        }
        outloop:
#if defined(DEBUG_LV2)
        cost_modif_detailed(clustering, i_star, j_star, &(clustering->p_c[point * config->NUM_DIM]), config);
#endif
        // Update cluster assignements
        update(clustering, i_star, j_star, point, config);
        cost += delta;
#if defined(DEBUG_LV1)
        const double real_cost = initial_cost(clustering, config);
        if (abs(cost - real_cost) > EPSILON) {
            std::cout << "Cost difference > " << EPSILON << std::endl;
            std::cout << "Real:" << real_cost << std::endl;
            std::cout << "Computed:" << cost << " with delta:" << delta << std::endl;
            clustering->print_with_ids(config, point, i_star, j_star);
            exit(1);
        }
#endif

        result->set_next_iter();
    }
    result->set_final_cost(cost);
    //result->set_final_clust(clustering->p_c, clustering->c_a);
}
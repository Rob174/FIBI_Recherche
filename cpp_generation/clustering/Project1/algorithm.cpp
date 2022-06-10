#include "algorithm.h"

#define UNINITIALIZED -1
void run(Clustering *clustering,Result*result, ImprovementChoice*impr, IterationOrder* order,Config *config)
{
    double cost = initial_cost(clustering, config);
    result->set_init_cost(cost);
    // Local seach
    result->set_time_start();
    bool improvement = true;
    while (improvement)
    {
        ClusteringChoice* choice = new ClusteringChoice();
        for (int point_moving_id = 0; point_moving_id < config->NUM_POINTS; point_moving_id++)
        {
            int from_cluster_id = clustering->c_a[point_moving_id];
            order->restart(from_cluster_id);
            int to_clust_id = order->next();
            while (to_clust_id != -1)
            {
                double modif = cost_modif(
                    clustering,
                    from_cluster_id, to_clust_id,
                    &(clustering->p_c[point_moving_id * config->NUM_DIM]), config);
                if (modif < choice->vij)
                {
                    impr->choose_solution(choice, clustering, modif, point_moving_id, from_cluster_id, to_clust_id);
                    if (impr->stop_loop())
                    {
                        goto outloop;
                    }
                }
                to_clust_id = order->next();
                result->set_next_iter_glob();
            }
            order->end_loop();
        }
    outloop:
        if (choice->i == UNINITIALIZED || choice->vij > -0.001)
            improvement = false;
        else
        {
            // Update cluster assignements
            impr->after_choice();
            update(clustering, choice->l, choice->j, choice->i, config);
            cost += choice->vij;
            /*
            if (cost < 0) {
                std::cout << "Cost negative for seed" << config->SEED << std::endl;
                exit(1);
            }
            */

            result->set_next_iter();
        }
        double time_micro_elapsed = result->get_time_elapsed();
        /*if (time_micro_elapsed > 1000000 * 15) {
            std::cout << "Error timeout ";
            //break;
        }*/
        delete choice;
    }
    result->set_final_cost(cost);
    /*
    if(verbose)
        result->print_results();
        */
}
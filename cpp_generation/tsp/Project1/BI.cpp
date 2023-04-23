#include "BI.h"
//#define DEBUG_LV1
//#define DEBUG_LV2
#define EPSILON 1e-5
void run_BI(Tour* tour, DistanceMatrix*dist, Result* res,Config* conf) {
	//Source algorithm: https://ars.els-cdn.com/content/image/1-s2.0-S0166218X05003070-gr1.jpg
	// Note we follow the algorithm with inclusive intervals and index starting at 1. 
	// The appropriate modification of the index (subtract 1) is done before each usage
	//2. Initial solution
	double tour_length = tour->get_cost(dist);
	res->set_init_cost(tour_length);
#if defined(DEBUG_LV1) || defined(DEBUG_LV2)
	tour->describe();
#endif
	//3. Best Improvement
	double delta = 0;
	// Utilitary function to keep same indexes as in the paper
	auto cyclic_id = [conf](int i) {
		return cycle_id(i,conf->NUM_POINTS);
	};

	int i_star, j_star;
	while (true) {
		delta = 0;
		for (int i = 0; i <= conf->NUM_POINTS - 3; i++) {
			int n_prime = i == 0 ? conf->NUM_POINTS - 2 : conf->NUM_POINTS-1;
			for (int j = i + 2; j <= n_prime; j++) {
				double delta_ij =
					dist->get(
						tour->at(cyclic_id(i),conf),
						tour->at(cyclic_id(j), conf)
					)
					+ dist->get(
						tour->at(cyclic_id(i+1), conf),
						tour->at(cyclic_id(j+1), conf))
					- dist->get(
						tour->at(cyclic_id(i), conf),
						tour->at(cyclic_id(i + 1), conf))
					- dist->get(
						tour->at(cyclic_id(j), conf),
						tour->at(cyclic_id(j + 1), conf));
				res->set_next_iter_glob();
				if (delta_ij < delta) {
					delta = delta_ij;
					i_star = i;
					j_star = j;
				}
			}
		}
		res->set_next_iter();
		if (abs(delta) < EPSILON) {
			// 5. Best tour found
			res->set_final_cost(tour_length);
			return;
		}
		if (tour_length < 0) {
			std::cout << "Error negative cost " << tour_length << std::endl;
			exit(1);
		}
		//4. Update: Exchange positions for the loops
		tour->apply_two_opt(i_star, j_star,conf);
		//Debug informations (LV1 = only stop when cost < 0 ; LV2 = print always infos)
#if defined(DEBUG_LV1) || defined(DEBUG_LV2)
		if (tour->check_tour() == 0) {
			std::cout << "Error tour " << std::endl;
			exit(1);
		}
		const int backup_tour_lenght = tour_length;
		const double real_length = tour->get_cost(dist);
#endif
		// Update cost
		tour_length += delta;
#if defined(DEBUG_LV1) && !defined(DEBUG_LV2)
		if (abs(real_length - tour_length) > 1e-9) {
			std::cout << "Error ";
#endif
#if defined(DEBUG_LV1) || defined(DEBUG_LV2)
			std::cout << "cost computation " << std::endl
				<< "Found: " << std::fixed << std::setprecision(16) << tour_length << " ; "
				<< "Real: " << std::fixed << std::setprecision(16) << real_length << std::endl
				<< "With delat: " << std::fixed << std::setprecision(16) << delta << std::endl
				<< "Diff observed: " << std::fixed << std::setprecision(16) << abs(real_length - tour_length) << std::endl
				<< "Previous tour length: " << std::fixed << std::setprecision(16) << backup_tour_lenght << std::endl
				<< "For tour: "
				<< std::flush;
			tour->describe(cyclic_id(i_star), cyclic_id(j_star+1));
			std::cout << "******************************************************************************************************" << std::endl;

#endif
#if defined(DEBUG_LV1) && !defined(DEBUG_LV2)
			exit(1);
		}
#endif
	}
	//Collect results
	res->set_final_cost(tour_length);
}
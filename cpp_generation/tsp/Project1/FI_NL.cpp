#pragma once
#include "FI_NL.h"
//#define DEBUG_LV1
//#define DEBUG_LV2
#define EPSILON 1e-5
void run_FI_NL(Tour* tour, DistanceMatrix* dist, Result* res, Config* conf) {
	//Source algorithm: https://ars.els-cdn.com/content/image/1-s2.0-S0166218X05003070-gr1.jpg
	// Note we follow the algorithm with inclusive intervals and index starting at 1. 
	// The appropriate modification of the index (subtract 1) is done before each usage
	//2. Initial solution
	double tour_length = tour->get_cost(dist);
	res->set_init_cost(tour_length);
	//3. Best Improvement
	double delta = 0;
	// Utilitary function to keep same indexes as in the paper
	auto cyclic_id = [conf](int i) {
		return cycle_id(i, conf->NUM_POINTS);
	};
	OrderedDistanceMatrix* dist_ordered = new OrderedDistanceMatrix(dist, conf);
	int i_star, j_star;
	int* posOfTownInTour = new int[conf->NUM_POINTS];
	for (int i = 0; i < conf->NUM_POINTS; i++) {
		posOfTownInTour[tour->at(i, conf)] = i;
	}
	while (true) {
		delta = 0;
		// Property used: if a and b are two leaving  edges and c and d two entering edges
		// If there is an improvement
		/*
		* Property used: if (T1,T2),(T3,T4) is changed into (T1,T4),(T2,T3) with an cost improvement
		* cost improvement means that : delta = [cost(TI+1,TJ) - cost(TI,TI+1)] + [cost(TI,TJ+1) - cost(TJ,TJ+1)] < 0 (= improvement, cost decreased)
		* which implies that either cost(TI,TI+1) > cost(TI+1,TJ) or cost(TJ,TJ+1) > cost(TI,TJ+1) or both
		 --TI--------TI+1--       --TI       TI+1-
		 |				  |      |    x    x      |
		...              ... => ...     x        ...
		 |				  |      |    x   x       |
		 --TJ--------TJ+1--      --TJ       TJ+1--

		 So by contraposition, if none of the above conditions are satisfied then it implies that there is no improvement
		 That is why we will check both inequalities
		*/
		// Inequality 1: cost(TI,TI+1) > cost(TI+1,TJ)
		// Select TI
		for (int i = 0; i <= conf->NUM_POINTS-1; i++) {
			/*
			As we must have cost(TI + 1, TJ) < cost(TI, TI + 1) = cost(TI + 1, TJ) < cost(TI + 1,TI)
			With the neighbouring list (nearest to farthest town for each town),
			We iterate from smallest to largest edge
			We can stop the search once we reach TI. For that we need to know the value of I
			We need to be able to map for a given source id town and a given destination id town what is its rank for source town
			*/
			int n_prime = dist_ordered->rank_of_edge(tour->at(cyclic_id(i),conf), tour->at(cyclic_id(i + 1),conf)) - 1;//pxi,xi+1 : xi -> tour->at
			for (int jprime = 1; jprime <= n_prime-1; jprime++) {
				const int c = dist_ordered->town_of_rank(
					tour->at(cyclic_id(i),conf),
					cyclic_id(jprime)				
				);
				int j = posOfTownInTour[c];
				if (i == j)
					continue;
				double delta_ij =
					dist->get(
						tour->at(cyclic_id(i), conf),
						tour->at(cyclic_id(j), conf)
					)
					+ dist->get(
						tour->at(cyclic_id(i + 1), conf),
						tour->at(cyclic_id(j + 1), conf)
					)
					- dist->get(
						tour->at(cyclic_id(i), conf),
						tour->at(cyclic_id(i + 1), conf)
					)
					- dist->get(
						tour->at(cyclic_id(j), conf),
						tour->at(cyclic_id(j + 1), conf)
					);
				res->set_next_iter_glob();
				if (delta_ij < -EPSILON) {
					delta = delta_ij;
					i_star = i;
					j_star = j;
					goto outloop;
				}
			}
		}
		// Inequality 2: cost(TJ,TJ+1) > cost(TI,TJ+1) which is equivalent to cost(TI-1,TI) > cost(TJ-1,TI)
		for (int i = 1; i <= conf->NUM_POINTS; i++) {
			int n_prime = dist_ordered->rank_of_edge(tour->at(cyclic_id(i),conf), tour->at(cyclic_id(i - 1),conf)) - 1;// final rank
			for (int jprime = 1; jprime <= n_prime-1; jprime++) {
				// We iterate over TJ-1 (j=j-1)
				int j = posOfTownInTour[
						dist_ordered->town_of_rank(
							tour->at(cyclic_id(i), conf),
							cyclic_id(jprime)
					)
				];
				if (j == 0)
					j = conf->NUM_POINTS;
				if (i == j)
					continue;
				double delta_ij =
					dist->get(
						tour->at(cyclic_id(i), conf),
						tour->at(cyclic_id(j), conf)
					)
					+ dist->get(
						tour->at(cyclic_id(i - 1), conf),
						tour->at(cyclic_id(j - 1), conf)
					)
					- dist->get(
						tour->at(cyclic_id(i), conf),
						tour->at(cyclic_id(i - 1), conf)
					)
					- dist->get(
						tour->at(cyclic_id(j), conf),
						tour->at(cyclic_id(j - 1), conf)
					);
				res->set_next_iter_glob();
				if (delta_ij < -EPSILON) {
					delta = delta_ij;
					i_star = i-1;
					j_star = j-1;
					goto outloop;
				}
			}
		}
		outloop:
		res->set_next_iter();
		if (abs(delta) < EPSILON) {
			// 5. Best tour found
			res->set_final_cost(tour_length);
			delete[] posOfTownInTour;
			delete dist_ordered;
			return;
		}
		//4. Update: Exchange positions for the loops
		tour->apply_two_opt(cyclic_id(i_star), cyclic_id(j_star), conf);
		for (int a = i_star + 1; a < i_star + 1 + cyclic_length(i_star + 1, j_star, conf->NUM_POINTS); a++) {
			const int l = cyclic_id(a);
			posOfTownInTour[tour->at(l,conf)] = l;
		}
		// Update cost
		tour_length += delta;
		if (tour_length < 0) {
			std::cout << "Error negative cost " << tour_length << std::endl;
			exit(1);
		}
		//Debug informations (LV1 = only stop when cost < 0 ; LV2 = print always infos)
#if defined(DEBUG_LV1) || defined(DEBUG_LV2)
		if (tour->check_tour() == 0) {
			std::cout << "Error tour " << std::endl;
			tour->describe(i_star, j_star + 1);
			exit(1);
		}
		const double real_length = tour->get_cost(dist);
#endif
#if defined(DEBUG_LV1) && !defined(DEBUG_LV2)
		if (abs(real_length - tour_length) > 1e-5) {
			std::cout << "Error ";
#endif
#if defined(DEBUG_LV1) || defined(DEBUG_LV2)
			std::cout << "cost computation " << std::endl
				<< "Found: " << tour_length << " ; "
				<< "Real: " << real_length << std::endl
				<< "For tour: "
				<< std::flush;
			tour->describe(i_star, j_star + 1);
			std::cout << "******************************************************************************************************" << std::endl;

#endif
#if defined(DEBUG_LV1) && !defined(DEBUG_LV2)
			exit(1);
		}
#endif
	}
	//Collect results
	res->set_final_cost(tour_length);
	delete[] posOfTownInTour;
	delete dist_ordered;
}
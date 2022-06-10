#include "tests.h"
//0 : pass ; 1 : fail
void print(std::string name) {
	std::cout << "\x1B[94mTest " << name<<"\033[0m" << std::endl;
}
void succ_result() {
	std::cout << "\x1B[32mPASS\033[0m" << std::flush << std::endl;
}
int test_distance_matrix() {
	print("test_distance_matrix");
	const int num_pts = 3;
	double points[2 * num_pts] = {
		0,0,
		0,1,
		2,2
	};
	double references[num_pts][num_pts] = {
		{0.,1.,2 * sqrt(2)},
		{1.,0.,sqrt(5)},
		{2 * sqrt(2),sqrt(5),0.}
	};
	Config conf;
	conf.NUM_DIM = 2;
	conf.NUM_POINTS = num_pts;
	DistanceMatrix* distances = new DistanceMatrix(&conf);
	distances->compute(points);
	for (int i = 0; i < conf.NUM_POINTS; i++) {
		for (int j = 0; j < conf.NUM_POINTS; j++) {
			const double value = distances->get(i, j);
			if (value != references[i][j]) {
				std::cout << "Computed " << value << " but reference " << references[i][j] << " diff : " << abs(value - references[i][j]);
				std::cout << " for points " << points[i * 2] << "," << points[i * 2 + 1] << " and " << points[j * 2] << "," << points[j * 2 + 1] << std::endl;
				return 1;
			}
		}
	}
	delete distances;
	return 0;
}

int test_generated_distance_matrix() {
	print("test_generated_distance_matrix");
	const int num_pts = 3;
	Config conf;
	conf.NUM_DIM = 2;
	conf.NUM_POINTS = num_pts;
	GeneratedDistanceMatrix*distances = new GeneratedDistanceMatrix(&conf);
	distances->compute();
	for (int i = 0; i < conf.NUM_POINTS; i++) {
		for (int j = 0; j < conf.NUM_POINTS; j++) {
			std::cout << "Distance " << i << " " << j << " : " << distances->get(i, j) << std::endl;
		}
	}
	delete distances;
	return 0;
}
int test_random_tour() {
	print("test_random_tour");
	Config conf;
	conf.NUM_POINTS = 10;
	conf.SEED_ASSIGN = 1;
	int* tour1 = random_tour(&conf);
	for (int i = 0; i < conf.NUM_POINTS; i++) {
		std::cout << tour1[i] << " ";
	}
	std::cout << std::endl;
	int* tour2 = random_tour(&conf);
	for (int i = 0; i < conf.NUM_POINTS; i++) {
		std::cout << tour2[i] << " ";
	}
	std::cout << std::endl;
	delete tour1;
	delete tour2;
	return 0;
}
int test_improved_tour() {
	print("test_improved_tour");
	Config conf;
	conf.NUM_POINTS = 10;
	conf.SEED_POINTS = 1;
	conf.SEED_ASSIGN = 1;
	GeneratedDistanceMatrix* distances = new GeneratedDistanceMatrix(&conf);
	distances->compute();
	int* tour1 = improved_tour(&conf,distances);
	for (int i = 0; i < conf.NUM_POINTS; i++) {
		std::cout << tour1[i] << " ";
	}
	std::cout << std::endl;
	int* tour2 = improved_tour(&conf, distances);
	for (int i = 0; i < conf.NUM_POINTS; i++) {
		std::cout << tour2[i] << " ";
	}
	std::cout << std::endl;
	delete tour1;
	delete tour2;
	return 0;
}
int test_open_tsp_lib() {
	print("test_open_tsp_lib");
	Config conf;
	conf.SEED_POINTS = 1;
	conf.NUM_DIM = 2;
	double *points = tsplib_sample_points(&conf);
	std::cout << conf.NUM_POINTS << " points charges" << std::endl;
	conf.SEED_POINTS = 2;
	points = tsplib_sample_points(&conf);
	std::cout << conf.NUM_POINTS << " points charges" << std::endl;
	return 0;
}
int test_tour_dist() {
	print("test_tour_dist");
	const int num_pts = 3;
	double points[2 * num_pts] = {
		0,0,
		0,1,
		2,2
	};
	Config conf;
	conf.NUM_DIM = 2;
	conf.NUM_POINTS = num_pts;
	conf.SEED_ASSIGN = 1;
	int init_tour[] = { 0,1,2 };
	Tour* t = new Tour(init_tour ,&conf);
	DistanceMatrix* distances = new DistanceMatrix(&conf);
	distances->compute(points);
	double tour_length = t->get_cost(distances);
	const double real_length = (1 + sqrt(5) + 2 * sqrt(2));
	if (tour_length != real_length) {
		std::cout << "Wrong distance " << tour_length << " and not the expected " << real_length << std::endl;
		return 1;
	}
	return 0;
}
int test_run_BI() {
	print("test_run_BI");
	Config conf;
	conf.NUM_DIM = 2;
	conf.NUM_POINTS = 50;
	conf.SEED_POINTS = 1;
	conf.SEED_ASSIGN = 1;

	Result* res = new Result(&conf);
	res->set_time_start();
	double* points = random_eucl_points_uniform(&conf);
	DistanceMatrix* distances = new DistanceMatrix(&conf);
	distances->compute(points);
	Tour* tour = new Tour(random_tour(&conf),&conf);
	run_BI(tour, distances,res, &conf);
	res->set_time_end();
	delete tour;
	delete distances;
	delete res;
	return 0;
}
int test_run_FI() {
	print("test_run_FI");
	Config conf;
	conf.NUM_DIM = 2;
	conf.NUM_POINTS = 50;
	conf.NUM_CLUST = 2;
	conf.SEED_POINTS = 1;
	conf.SEED_ASSIGN = 1;
	for (int i = 0; i < 3; i++) {
		conf.IT_ORDER = 0;

		Result* res = new Result(&conf);
		res->set_time_start();
		double* points = random_eucl_points_uniform(&conf);
		DistanceMatrix* distances = new DistanceMatrix(&conf);
		distances->compute(points);
		Tour* tour = new Tour(random_tour(&conf), &conf);
		run_FI(tour, distances, res,&conf);
		res->set_time_end();
		delete tour;
		delete distances;
		delete res;
	}
	return 0;
}
void run_tests() {
	std::vector<int (*)()> test_functions = {
		test_distance_matrix,
		test_generated_distance_matrix,
		test_random_tour,
		test_improved_tour,
		test_open_tsp_lib,
		test_tour_dist,
		test_run_BI,
		test_run_FI
	};
	for (auto& f : test_functions) {
		if (f() != 0)
			break;
		else {
			succ_result();
		}
	}

}
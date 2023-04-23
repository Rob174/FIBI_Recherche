#include "preprocessing.h"

DistanceMatrix::DistanceMatrix(Config* config) : conf(config) {
	this->d = new double* [config->NUM_POINTS-1];
	this->initialized = false;
}
DistanceMatrix::~DistanceMatrix()
{
	for (int i_pt = 1; i_pt < this->conf->NUM_POINTS; i_pt++) {// 1 not an error (distance from a point to the same point trivial)
		delete this->d[i_pt-1];
	}
	delete this->d;
}


void DistanceMatrix::compute(double* points) {
	for (int i_pt = 1; i_pt < this->conf->NUM_POINTS; i_pt++) {// 1 not an error (distance from a point to the same point trivial)
		this->d[i_pt - 1] = new double[i_pt];
		for (int j_pt = 0; j_pt < i_pt; j_pt++) {// distance from a point to the same point trivial so excluded here
			this->d[i_pt - 1][j_pt] = dist(&points[i_pt * this->conf->NUM_DIM], &points[j_pt * this->conf->NUM_DIM], this->conf);
		}
	}
	this->initialized = true;
}

void GeneratedDistanceMatrix::compute() {
	const double MIN_DIST = 0, MAX_DIST = 100;
	std::mt19937 gen_dist(this->conf->SEED_POINTS);
	std::uniform_real_distribution<double> dis_distances(MIN_DIST, MAX_DIST);
	for (int i_pt = 1; i_pt < this->conf->NUM_POINTS; i_pt++) {// 1 not an error (distance from a point to the same point trivial)
		this->d[i_pt - 1] = new double[i_pt];
		for (int j_pt = 0; j_pt < i_pt; j_pt++) {// distance from a point to the same point trivial so excluded here
			this->d[i_pt - 1][j_pt] = dis_distances(gen_dist);
		}
	}
	this->initialized = true;
}

double DistanceMatrix::get(int pt1_idx, int pt2_idx) {
	if (!this->initialized) {
		std::cout << "compute method not called" << std::endl;
		exit(1);
	}
	if (pt1_idx >= this->conf->NUM_POINTS || pt1_idx < 0) {
		std::cout << "Wrong pt1_idx with :" << pt1_idx << std::endl;
		exit(1);
	}
	if (pt2_idx >= this->conf->NUM_POINTS || pt2_idx < 0) {
		std::cout << "Wrong pt2_idx with :" << pt2_idx << std::endl;
		exit(1);
	}
	int id1, id2;
	if (pt2_idx == pt1_idx)
		return 0;
	else if (pt2_idx > pt1_idx) {
		id1 = pt2_idx;
		id2 = pt1_idx;
	}
	else {
		id1 = pt1_idx;
		id2 = pt2_idx;
	}
	// Correct ids due to the fact that we erased the first line of the distance matrix
	id1--;
	return this->d[id1][id2];
}
class CustomComparitor
{
public:
	int operator()(const std::pair<int, double>& lhs, const std::pair<int, double>& rhs) const
	{
		if (lhs.second != rhs.second)
			return lhs.second < rhs.second;
		else
			return lhs.first < rhs.first;
	}
};
OrderedDistanceMatrix::OrderedDistanceMatrix(DistanceMatrix* distances, Config* conf)
{
	this->conf = conf;
	using namespace std;
	// Init arrays
	this->d_prime = new double* [conf->NUM_POINTS];
	this->r = new int* [conf->NUM_POINTS];//indices of ranked edges
	this->p = new int* [conf->NUM_POINTS];
	for (int i = 0; i < conf->NUM_POINTS; i++) {
		// Sort towns nearest to farthest
		set<pair<int, double>, CustomComparitor> nearest_towns_of_i;
		for (int j = 0; j < conf->NUM_POINTS; j++) {
			nearest_towns_of_i.insert(make_pair(j,distances->get(i,j)));
		}
		// Init arrays
		this->d_prime[i] = new double [conf->NUM_POINTS];
		this->r[i] = new int [conf->NUM_POINTS];
		this->p[i] = new int[conf->NUM_POINTS];
		int j = 0;
		for (int j = 0; j < conf->NUM_POINTS; j++) {
			this->d_prime[i][j] = -19999999.;
			this->r[i][j] = -19999999;
			this->p[i][j] = -19999999;
		}
		for (auto elem : nearest_towns_of_i)
		{
			// Fill array
			this->d_prime[i][j] = elem.second; // D' has at i,j the j-th closest town from the i-th town
			this->r[i][j] = elem.first; // R has at i,j the id of the j-th closest town from the i-th town
			this->p[i][elem.first] = j; // P has at i,j the rank of the town of id j in the ranked edges
			j++;
		}
	}
	const int b = 0;
}

OrderedDistanceMatrix::~OrderedDistanceMatrix()
{
	for (int i = 0; i < this->conf->NUM_POINTS; i++) {
		delete[] this->d_prime[i];
		delete[] this->r[i];
		delete[] this->p[i];
	}
	delete[] this->d_prime;
	delete[] this->r;
	delete[] this->p;
}
int OrderedDistanceMatrix::rank_of_edge(int from_town, int rank) {
	return this->p[from_town][rank];
}
int OrderedDistanceMatrix::town_of_rank(int from_town, int to_town)
{
	return this->r[from_town][to_town];
}

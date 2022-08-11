#include "results.h"

void Result::set_next_iter()
{
	this->num_iter++;
}

void Result::set_next_iter_glob()
{
	this->num_iter_glob++;
}

void Result::set_final_cost(double final_cost)
{
	this->final_cost = final_cost;
}
void Result::set_init_cost(double init_cost)
{
	this->init_cost = init_cost;
}
double Result::get_time_elapsed() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - this->time_start).count();;
}
void Result::set_time_end()
{
	this->duration = this->get_time_elapsed();
}

void Result::set_time_start()
{
	this->time_start = std::chrono::steady_clock::now();
}
void Result::print_results() {
	using namespace std;
	cout << endl;
	cout	<< ",NUM_POINTS:" << this->config->NUM_POINTS
			<< ",NUM_DIM:" << this->config->NUM_DIM
			<< ",POINTS_DISTR:" << this->config->POINTS_DISTR
			<< ",NUM_CLUST:" << this->config->NUM_CLUST
			<< ",CLUST_IMPR:" << this->config->CLUST_IMPR
			<< ",IMPR_CLASS:" << this->config->IMPR_CLASS
			<< ",IT_ORDER:" << this->config->IT_ORDER
			<< ",SEED_POINTS:" << this->config->SEED_POINTS
			<< ",SEED_ASSIGN:" << this->config->SEED_ASSIGN
			<< ",init_cost:" << fixed << setprecision(3) << this->init_cost
			<< ",end_cost:" << this->final_cost 
			<< ",num_iter:" << this->num_iter 
			<< ",num_iter_tot:" << this->num_iter_glob 
			<< ",time:" << this->duration 
			<< endl;
}
std::vector<double>* Result::get_result() {
	std::vector<double>* to_backup = new std::vector<double>(); 
	to_backup->push_back((double)this->config->NUM_POINTS);
	to_backup->push_back((double)this->config->NUM_DIM);
	to_backup->push_back((double)this->config->POINTS_DISTR);
	to_backup->push_back((double)this->config->NUM_CLUST);
	to_backup->push_back((double)this->config->CLUST_IMPR);
	to_backup->push_back((double)this->config->IMPR_CLASS);
	to_backup->push_back((double)this->config->IT_ORDER);
	to_backup->push_back((double)this->config->SEED_POINTS);
	to_backup->push_back((double)this->config->SEED_ASSIGN);
	to_backup->push_back((double)this->config->SEED_GLOB);
	to_backup->push_back((double)this->init_cost);
	to_backup->push_back((double)this->final_cost);
	to_backup->push_back((double)this->num_iter);
	to_backup->push_back((double)this->num_iter_glob);
	to_backup->push_back((double)this->duration);
	return to_backup;
}

void Result::set_final_clust(double*finalPoints, int*finalAssign) {
	this->finalPoints = finalPoints;
	this->finalAssign = finalAssign;
}


double* Result::get_points() {
	return this->finalPoints;
}
int* Result::get_finalAssign() {
	return this->finalAssign;
}

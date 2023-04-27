#include "Metrics.h"

double Metrics::set_final_time()
{
    this->duration = std::chrono::duration_cast<std::chrono::microseconds>(this->get_time() - this->time_start).count();
    return this->duration;
}
std::chrono::steady_clock::time_point Metrics::get_time()
{
    return std::chrono::steady_clock::now();
}
std::vector<std::string> Metrics::get_group()
{
    return std::vector<std::string>{"metadata"};
}

std::vector<std::vector<double>> Metrics::get_data()
{
    return std::vector<std::vector<double>>{ {
            (double)num_iter,
                (double)num_iter_glob,
                init_cost,
                final_cost,
                duration}};
}
std::vector<std::string> Metrics::get_data_names()
{
    return std::vector<std::string>{ 
            "num_iter",
                "num_iter_glob",
                "init_cost",
                "final_cost",
                "duration"};
}

void Metrics::print() {
    std::cout << "Metrics:";
    std::vector<std::string> names{ "num_iter","num_iter_glob","init_cost","final_cost","duration" };
    std::vector<double> values = this->get_data()[0];
    for (int i = 0; i < names.size(); i++) {
        if (i != 0) {
            std::cout << ",";
        }
        std::cout << values[i];
    }
    std::cout << std::endl;
}

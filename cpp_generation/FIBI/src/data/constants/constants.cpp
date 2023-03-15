#include "./constants.h"

/******************************OUT HDF5*************************************/
std::vector<int> ClusteringConfig::get_values()
{
    std::vector<int> values{
        this->DATASET.get(),
        this->FI_BI.get(),
        this->IMPR.get(),
        this->NUM_CLUST.get(),
        this->NUM_DIM.get(),
        this->NUM_POINTS.get(),
        this->SEED_ASSIGN.get(),
        this->SEED_GLOB.get(),
        this->SEED_PROBLEM.get()
    };
    return values;
}
std::vector<int> TSPConfig::get_values()
{
    std::vector<int> values{
        this->DATASET.get(),
        this->FI_BI.get(),
        this->IMPR.get(),
        this->NUM_DIM.get(),
        this->NUM_TOWNS.get(),
        this->SEED_ASSIGN.get(),
        this->SEED_GLOB.get(),
        this->SEED_PROBLEM.get() };
    return values;
}
std::vector<int> MAXSATConfig::get_values()
{
    std::vector<int> values{
        this->DATASET.get(),
        this->FI_BI.get(),
        this->IMPR.get(),
        this->NUM_CLAUSES.get(),
        this->NUM_VARIABLES.get(),
        this->SEED_ASSIGN.get(),
        this->SEED_GLOB.get(),
        this->SEED_PROBLEM.get()
    };
    return values;
}
std::map<std::string,int> ClusteringConfig::get_constants()
{
    std::map<std::string,int> values{
        {"SEED_GLOB", this->SEED_GLOB.get()},
        {"SEED_PROBLEM", this->SEED_PROBLEM.get()},
        {"SEED_ASSIGN", this->SEED_ASSIGN.get()},
        {"DATASET", this->DATASET.get()},
        {"FI_BI", this->FI_BI.get()},
        {"IMPR", this->IMPR.get()},
        {"NUM_POINTS", this->NUM_POINTS.get()},
        {"NUM_DIM", this->NUM_DIM.get()},
        {"NUM_CLUST", this->NUM_CLUST.get()}};
    return values;
}
std::map<std::string, int> TSPConfig::get_constants()
{
    std::map<std::string,int> values{
        {"SEED_GLOB", this->SEED_GLOB.get()},
        {"SEED_PROBLEM", this->SEED_PROBLEM.get()},
        {"SEED_ASSIGN", this->SEED_ASSIGN.get()},
        {"DATASET", this->DATASET.get()},
        {"FI_BI", this->FI_BI.get()},
        {"IMPR", this->IMPR.get()},
        {"NUM_TOWNS", this->NUM_TOWNS.get()},
        {"NUM_DIM", this->NUM_DIM.get()}};
    return values;
}
std::map<std::string, int> MAXSATConfig::get_constants()
{
    std::map<std::string,int> values{
        {"SEED_GLOB", this->SEED_GLOB.get()},
        {"SEED_PROBLEM", this->SEED_PROBLEM.get()},
        {"SEED_ASSIGN", this->SEED_ASSIGN.get()},
        {"DATASET", this->DATASET.get()},
        {"FI_BI", this->FI_BI.get()},
        {"IMPR", this->IMPR.get()},
        {"NUM_VARIABLES", this->NUM_VARIABLES.get()},
        {"NUM_CLAUSES", this->NUM_CLAUSES.get()}
    };
    return values;
}
std::string ClusteringConfig::get_key()
{
    std::vector<int> v = this->get_values();
    // Convert to string
    std::vector<std::string> values;
    for (auto i : v) {
        values.push_back(std::to_string(i));
    }
    return join(values, ",");
}
std::string TSPConfig::get_key() {
    std::vector<int> v = this->get_values();
    // Convert to string
    std::vector<std::string> values;
    for (auto i : v) {
        values.push_back(std::to_string(i));
    }
    return join(values, ",");
}
std::string MAXSATConfig::get_key() {
    std::vector<int> v = this->get_values();
    // Convert to string
    std::vector<std::string> values;
    for (auto i : v) {
        values.push_back(std::to_string(i));
    }
    return join(values, ",");
}

/*************************************UTILS**********************************/
void ClusteringConfig::print()
{   
    std::vector<std::string> v;
    for (auto i : this->get_constants()) {
        v.push_back(i.first + ": " + std::to_string(i.second));
    }
    std::cout << join(v, ", ") << std::endl;
}
void TSPConfig::print() {
    std::vector<std::string> v;
    for (auto i : this->get_constants()) {
        v.push_back(i.first + ": " + std::to_string(i.second));
    }
    std::cout << join(v, ", ") << std::endl;
}
void MAXSATConfig::print() {
    std::vector<std::string> v;
    for (auto i : this->get_constants()) {
        v.push_back(i.first + ": " + std::to_string(i.second));
    }
    std::cout << join(v, ", ") << std::endl;
}

std::vector<std::string> AbstractConfig::get_group()
{
    return std::vector<std::string>{"metadata"};
}
std::vector<std::vector<double>> AbstractConfig::get_data()
{
    std::vector<int> data = this->get_values();
    std::vector<double> values(data.begin(), data.end());
    return std::vector<std::vector<double>>{values};
}
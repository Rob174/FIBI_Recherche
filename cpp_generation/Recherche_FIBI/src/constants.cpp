#include "constants.h"

AbstractConfig::AbstractConfig(std::map<const char *, int> *args)
{
    this->constants = args;
}

bool ClusteringConfig::checkArgsValid(bool blocking)
{

    const char *expected_keys[] = {
        "NUM_POINTS",
        "NUM_DIM",
        "POINTS_DISTR",
        "NUM_CLUST",
        "CLUST_IMPR",
        "IMPR_CLASS",
        "IT_ORDER",
        "SEED_POINTS",
        "SEED_ASSIGN",
        "SEED_GLOB"};
    for (const char *a : expected_keys)
    {
        if (this->constants->find(a) == this->constants->end())
        {
            std::cout << "Constant not found: " << a << std::endl;
            if (blocking)
            {
                throw new KeyNotFound(a);
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

void AbstractConfig::print()
{
    std::string first = ",";
    for (auto const &x : *constants)
    {
        std::cout << first << x.first << ":" << x.second;
        first = "";
    }
    std::cout << std::endl;
}

std::vector<int> AbstractConfig::get_values()
{
    std::vector<int> values;
    for (auto const &x : *constants)
    {
        int v = x.second;
        values.push_back(v);
    }
    return values;
}

int AbstractConfig::get(const char *name)
{
    if (this->constants->find(name) == this->constants->end())
    {
        throw new KeyNotFound(name);
    }
}

bool TSPConfig::checkArgsValid(bool blocking)
{

    const char *expected_keys[] = {
        "NUM_POINTS ",
        "NUM_CLUST",
        "NUM_DIM",
        "IMPR_CLASS ",
        "IT_ORDER",
        "SEED_POINTS ",
        "SEED_ASSIGN ",
        "DATASET  ",
        "TOUR_ALGO",
        "SEED_GLOB"};
    for (const char *a : expected_keys)
    {
        if (this->constants->find(a) == this->constants->end())
        {
            std::cout << "Constant not found: " << a << std::endl;
            if (blocking)
            {
                throw new KeyNotFound(a);
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

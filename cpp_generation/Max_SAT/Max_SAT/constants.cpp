#include "constants.h"


/****************************************Checking the constants in the initialization phase*****************************************************/
void AbstractConfig::checkConstList(std::vector<std::string> expected, std::set<std::string> &keys_not_seen, std::string additionnal_infos)
{
    for (auto &c_exp : expected)
    {
        if (keys_not_seen.find(c_exp) == keys_not_seen.end())
        {
            if (this->possible_values.find(c_exp) == this->possible_values.end()) {
                THROW_EXC4("Mandatory key ", c_exp, " not found ", additionnal_infos, 0);
            }
            else {
                THROW_EXC4("Mandatory key ", c_exp, " not found ", additionnal_infos, 20);
            }
        } else {
            keys_not_seen.erase(c_exp);
        }
    }
}

void AbstractConfig::check_initialized(bool bypass_secur)
{
    if (!checked && !bypass_secur)
    {
        THROW_EXC1("Constants must be checked before usage of any function",1);
    }
}

void AbstractConfig::check_and_fill_keys_after_load_instance()
{
    this->check_keys_after_load_instance();
    this->checked = true;
}

/*****************************************SET AND GET CONSTANTS*************************************************/
void AbstractConfig::update_keys(std::map<std::string, int> new_v)
{
    for (auto const &x : new_v)
    {
        std::map<std::string, int>::iterator it = this->constants->find(x.first);
        if (it == this->constants->end())
        {
            this->constants->insert(std::make_pair(x.first, x.second));
        }
        else
        {
            it->second = x.second;
        }
    }
}
int AbstractConfig::get(std::string name, bool bypass_secur)
{
    this->check_initialized(bypass_secur);
    if (this->constants->find(name) == this->constants->end())
    {
        if (this->possible_values.contains(name)) {
            return -1;
        }
        throw new KeyNotFound(name.c_str());
    }
    return this->constants->at(name);
}
std::vector<std::string> AbstractConfig::get_values_names() {
    this->check_initialized();
    std::vector<std::string> values;
    for (auto& constName : this->possible_values)
    {
        values.push_back(constName);
    }
    return values;
}
/******************************OUT HDF5*************************************/
std::vector<int> AbstractConfig::get_values()
{
    this->check_initialized();
    std::vector<int> values;
    for (auto &constName : this->possible_values)
    {
        try {
            values.push_back(this->constants->at(constName));
        }
        catch (...) {
            THROW_EXC2("Cannot find key ", constName, 2);
        }
    }
    return values;
}
std::string AbstractConfig::get_key()
{
    this->check_initialized();
    std::vector<int> consts = this->get_values();
    std::vector<std::string> values;
    std::transform(std::begin(consts), std::end(consts), std::back_inserter(values), [](int d)
                   { return std::to_string(d); });
    return join(values, ",");
}

/*************************************UTILS**********************************/
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

std::vector<std::string> AbstractConfig::get_group()
{
    this->check_initialized();
    return std::vector<std::string>{"metadata"};
}
std::vector<std::vector<double>> AbstractConfig::get_data()
{
    this->check_initialized();
    std::vector<int> data = this->get_values();
    std::vector<double> values(data.begin(), data.end());
    return std::vector<std::vector<double>>{values};
}
#define M_reg_const(key,descr) this->possible_values.insert(stringify(key));this->description.insert(std::make_pair(stringify(key),descr))
AbstractConfig::AbstractConfig(std::map<std::string, int>* args)
{
    M_reg_const(SEED_GLOB, "Global seed to know the status in the global loop. Must be uniq");
    M_reg_const(SEED_PROBLEM, "Seed to generate the clauses or to choose the instance");
    M_reg_const(SEED_ASSIGN, "Seed to generate the affectations");
    M_reg_const(DATASET, "Which dataset to use. Available: 0: Generated dataset with unequal weights; 1: Generated dataset with equal weights; 2:maxsat_benchmark; 3: test dataset to mimic maxsat bechmark");
    M_reg_const(FI_BI, "Choose if want to execute FI (0) or BI (1)");
    M_reg_const(IMPR, "Choose if want improve the initialization (restart the initialization and improve it)");
    this->constants = args;
    this->checked = false;
}
MAXSATConfig::MAXSATConfig(std::map<std::string, int> *args) : AbstractConfig(args) {

    M_reg_const(NUM_VARIABLES, "Number of variables in all clauses");
    M_reg_const(NUM_CLAUSES, "Number of clauses");
}
TSPConfig::TSPConfig(std::map<std::string, int> *args) : AbstractConfig(args)
{

    M_reg_const(NUM_TOWNS, "Number of towns");
    M_reg_const(NUM_DIM, "Number of dimensions for coordinates of towns");
}

ClusteringConfig::ClusteringConfig(std::map<std::string, int> *args) : AbstractConfig(args)
{
    M_reg_const(NUM_POINTS, "Number of points to cluster");
    M_reg_const(NUM_DIM, "Number of dimensions for coordinates of points");
    M_reg_const(NUM_CLUST, "Number of clusters to build");
}
void ClusteringConfig::check_keys_before_load_instance()
{
    std::vector<std::string> mandatory_subscase{
        stringify(SEED_GLOB),
        stringify(SEED_PROBLEM),
        stringify(SEED_ASSIGN),
        stringify(IMPR),
        stringify(DATASET),
        stringify(FI_BI)
        };
    std::set<std::string> possible_keys = std::set<std::string>{this->possible_values.begin(),this->possible_values.end()};
    this->checkConstList(mandatory_subscase,possible_keys);
    if (this->get(stringify(DATASET), true) == 0)
    {
        std::vector<std::string> mandatory_subscase{
            stringify(NUM_POINTS),
            stringify(NUM_DIM),
            stringify(NUM_CLUST)
            };
        std::set<std::string> possible_keys = std::set<std::string>{this->possible_values.begin(),this->possible_values.end()};
        this->checkConstList(mandatory_subscase,possible_keys);
    }
}

void ClusteringConfig::check_keys_after_load_instance()
{
    std::vector<std::string> mandatory{
        stringify(NUM_POINTS),
        stringify(NUM_DIM),
        stringify(NUM_CLUST),
        stringify(DATASET),
        stringify(FI_BI),
        stringify(IMPR),
        stringify(SEED_GLOB),
        stringify(SEED_PROBLEM),
        stringify(SEED_ASSIGN)
        };
    
    std::set<std::string> missingKeys = std::set<std::string>{this->possible_values.begin(),this->possible_values.end()};
    this->checkConstList(mandatory,missingKeys);

    if (missingKeys.size() > 0)
    {
        std::vector<std::string> keys(missingKeys.begin(), missingKeys.end());
        THROW_EXC2("Some constants provided will not be used: ", join(keys, ", "), 4);
    }
}

void TSPConfig::check_keys_before_load_instance()
{
    std::vector<std::string> mandatory{
        stringify(NUM_DIM),
        stringify(DATASET),
        stringify(FI_BI),
        stringify(IMPR),
        stringify(SEED_GLOB),
        stringify(SEED_PROBLEM),
        stringify(SEED_ASSIGN)
        };
    
    std::set<std::string> keys_not_seen;
    for (auto &c : *this->constants)
    {
        keys_not_seen.insert(c.first);
    }
    this->checkConstList(mandatory,keys_not_seen,"mandatory keys TSP");
}
void TSPConfig::check_keys_after_load_instance()
{
    std::vector<std::string> mandatory{
        stringify(NUM_TOWNS),
        stringify(NUM_DIM),
        stringify(DATASET),
        stringify(FI_BI),
        stringify(IMPR),
        stringify(SEED_GLOB),
        stringify(SEED_PROBLEM),
        stringify(SEED_ASSIGN)
        };
    std::set<std::string> missingKeys = std::set<std::string>{this->possible_values.begin(),this->possible_values.end()};
    this->checkConstList(mandatory,missingKeys);

    if (missingKeys.size() > 0)
    {
        std::vector<std::string> keys(missingKeys.begin(), missingKeys.end());
        THROW_EXC2("Some constants provided will not be used: ", join(keys, ", "), 4);
    }
}

void MAXSATConfig::check_keys_before_load_instance()
{
    // Mandatory keys

    std::vector<std::string> mandatory{
        stringify(SEED_GLOB),
        stringify(SEED_PROBLEM),
        stringify(SEED_ASSIGN),
        stringify(DATASET),
        stringify(IMPR),
        stringify(FI_BI)
    };
    std::set<std::string> keys_not_seen;
    for (auto &c : *this->constants)
    {
        keys_not_seen.insert(c.first);
    }
    this->checkConstList(mandatory,keys_not_seen,"mandatory keys MAXSAT");
#define M1_cst_force(key) this->get(stringify(key), true)
    if (M1_cst_force(DATASET) == 0)
    {
        std::vector<std::string> mandatory_subscase{
            stringify(NUM_VARIABLES),
            stringify(NUM_CLAUSES),
            };
        this->checkConstList(mandatory_subscase,keys_not_seen,"DATASET 0 or 1");
    }
    if (keys_not_seen.size() > 0)
    {
        std::vector<std::string> keys(keys_not_seen.begin(), keys_not_seen.end());
        THROW_EXC2("Some constants provided will not be used: ", join(keys, ", "),3);
    }
}

void MAXSATConfig::check_keys_after_load_instance()
{
    std::vector<std::string> mandatory {
        stringify(SEED_GLOB),
        stringify(SEED_PROBLEM),
        stringify(SEED_ASSIGN),
        stringify(DATASET),
        stringify(IMPR),
        stringify(FI_BI),
        stringify(NUM_VARIABLES),
        stringify(NUM_CLAUSES)
    };
    std::set<std::string> missingKeys = std::set<std::string>{this->possible_values.begin(),this->possible_values.end()};
    this->checkConstList(mandatory,missingKeys);

    if (missingKeys.size() > 0)
    {
        std::vector<std::string> keys(missingKeys.begin(), missingKeys.end());
        THROW_EXC2("Some constants provided will not be used: ", join(keys, ", "), 4);
    }
}
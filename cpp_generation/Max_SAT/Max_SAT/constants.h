#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <algorithm>

#include "exceptions.h"
#include "AbstractWritable.h"
#include "utils.h"


/*
 BEFORE PROBLEM GENERATION


                       MAXSAT                     Clustering                                     TSP
                          │
                     ┌────┴───────┐               ┌─────────────┐                          ┌─────────────┐
                     │SEED_GLOB   │               │ SEED_GLOB   │                          │ SEED_GLOB   │
                     │SEED_PROBLEM│               │ SEED_PROBLEM│                          │ SEED_PROBLEM│
                     │SEED_ASSIGN │               │ SEED_ASSIGN │                          │ SEED_ASSIGN │
                     │DATASET     │               │ DATASET     │                          │ DATASET     │
                     │IMPR        │               │ IMPR        │                          │ IMPR        │
                     │FI_BI       │               │ FI_BI       │                          │ FI_BI       │
                     └────┬───────┘               └─────┬───────┘                          └─────┬───────┘
                          │                             │                                        │
                          │                             ▼                                        ▼
                  ┌───────┴──────┐       ┌──── uniform_points  ───┐               ┌──── uniform_points  ───┐
                  ▼              ▼       │                        │               │                        │
           generated_clauses   ┌───┐     │   ┌──────────┐         ▼               │   ┌─────────┐          ▼
           │                   │   │     └──►│NUM_POINTS│        ┌──┐             └──►│NUM_TOWNS│         ┌──┐
           └►┌─────────────┐   └───┘         │NUM_DIM   │        │  │                 └─────────┘         │  │
             │NUM_VARIABLES│                 │NUM_CLUST │        └──┘                                     └──┘
             │NUM_CLAUSES  │                 └──────────┘
             └─────────────┘
                         xx                            x                                     x
                          xx                           xx                                  xxx
        ──────────────────xx────────────────────────────x─────────────────────────────────xx───────────
                          x                            xx                                 x
AFTER      ┌──────────────┐                  ┌────────────┐                       ┌────────────┐
           │ SEED_GLOB    │                  │SEED_GLOB   │                       │SEED_GLOB   │
           │ SEED_PROBLEM │                  │SEED_PROBLEM│                       │SEED_PROBLEM│
           │ SEED_ASSIGN  │                  │SEED_ASSIGN │                       │SEED_ASSIGN │
           │ DATASET      │                  │DATASET     │                       │DATASET     │
           │ IMPR         │                  │IMPR        │                       │IMPR        │
           │ FI_BI        │                  │FI_BI       │                       │FI_BI       │
           │ NUM_VARIABLES│                  │NUM_POINTS  │                       │NUM_TOWNS   │
           │ NUM_CLAUSES  │                  │NUM_DIM     │                       └────────────┘
           └──────────────┘                  │NUM_CLUST   │
                                             └────────────┘
*/



#define stringify(name) #name
#define M_cst_frc(name) conf->get(stringify(name),true)
#define M_cst(name) conf->get(stringify(name))
#define M_insert_cst(name,v) args.insert(std::make_pair(to_string(stringify(name)), v))
enum GlobalConst {
    SEED_GLOB,
    SEED_PROBLEM,
    SEED_ASSIGN,
    DATASET,
    FI_BI,
    IMPR
};
enum ClusteringConstantsNames
{
    NUM_POINTS,
    NUM_DIM,
    NUM_CLUST
};
enum MAXSATConstantsNames
{
    NUM_VARIABLES,
    NUM_CLAUSES
};

class AbstractConfig : public AbstractWritable<double>
{
protected:
    std::map<std::string, int> *constants;
    std::set<std::string> possible_values;
    std::map<std::string,std::string> description;
    bool checked;

public:
    AbstractConfig(std::map<std::string, int> *args);

    void checkConstList(std::vector<std::string> vals, std::set<std::string> &keys_not_seen, std::string infos = "");
    void check_initialized(bool bypass_secur = false);
    virtual void check_keys_before_load_instance() = 0;
    virtual void check_keys_after_load_instance() = 0;
    void check_and_fill_keys_after_load_instance();

    void update_keys(std::map<std::string, int>);
    int get(std::string, bool bypass_secur = false);

    virtual int num_choices() = 0;
    std::string get_key();
    std::vector<int> get_values();
    std::map<std::string, int> * get_constants(){return this->constants;};
    std::vector<std::string> get_values_names();

public:
    void print();
    std::vector<std::string> get_group();
    std::vector<std::vector<double>> get_data();
};
class ClusteringConfig : public AbstractConfig
{

public:
    ClusteringConfig(std::map<std::string, int> *args);
    int num_choices()
    {
        return this->constants->at(stringify(NUM_POINTS));
    }
    void check_keys_before_load_instance();
    void check_keys_after_load_instance();
};
class TSPConfig : public AbstractConfig
{
public:
    TSPConfig(std::map<std::string, int> *args);
    int num_choices()
    {
        return this->constants->at(stringify(NUM_POINTS));
    }
    void check_keys_before_load_instance();
    void check_keys_after_load_instance();
};
class MAXSATConfig : public AbstractConfig
{
public:
    MAXSATConfig(std::map<std::string, int> *args);
    int num_choices()
    {
        if (!checked)
        {
            throw GenericException(std::vector<std::string>{"Constants must be checked before usage of any function"});
        }
        return this->constants->at(stringify(NUM_VARIABLES));
    }
    void check_keys_before_load_instance();
    void check_keys_after_load_instance();
};
#endif
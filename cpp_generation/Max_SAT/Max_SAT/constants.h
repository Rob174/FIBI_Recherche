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
                     │SEED_PROBLEMLEM│               │ SEED_PROBLEMLEM│                          │ SEED_PROBLEMLEM│
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
           │ SEED_PROBLEMLEM │                  │SEED_PROBLEMLEM│                       │SEED_PROBLEMLEM│
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
class UninitializedInt
{
private:
    int value;
    bool initialized = false;

public:
    UninitializedInt(int value) : value(value), initialized(true) {};
    UninitializedInt() : value(-1), initialized(false) {};
    int get()
    {
        if (!initialized)
        {
            throw std::runtime_error("Uninitialized variable accessed");
        }
        return value;
    }

    void set(int v)
    {
        value = v;
        initialized = true;
    }
};
class AbstractConfig : public AbstractWritable<double>
{

public:
    AbstractConfig() {};

    virtual int num_choices() = 0;
    virtual std::string get_key() = 0;
    virtual std::vector<int> get_values() = 0;
    virtual std::map<std::string, int> get_constants() = 0;

public:
    virtual void print() = 0;
    std::vector<std::string> get_group();
    std::vector<std::vector<double>> get_data();
};
class ClusteringConfig : public AbstractConfig
{
public:
    UninitializedInt SEED_GLOB,
        SEED_PROBLEM,
        SEED_ASSIGN,
        DATASET,
        FI_BI,
        IMPR,
        NUM_POINTS,
        NUM_DIM,
        NUM_CLUST;

public:
    ClusteringConfig(std::map<std::string, int> args)
    {
        SEED_GLOB = UninitializedInt(args["SEED_GLOB"]);
        SEED_PROBLEM = UninitializedInt(args["SEED_PROBLEM"]);
        SEED_ASSIGN = UninitializedInt(args["SEED_ASSIGN"]);
        DATASET = UninitializedInt(args["DATASET"]);
        FI_BI = UninitializedInt(args["FI_BI"]);
        IMPR = UninitializedInt(args["IMPR"]);
        try
        {
            NUM_POINTS = UninitializedInt(args["NUM_POINTS"]);
        }
        catch (std::exception e)
        {
            // No problem, will be set later
        }
        try
        {
            NUM_DIM = UninitializedInt(args["NUM_DIM"]);
        }
        catch (std::exception e)
        {
            // No problem, will be set later
        }
        try
        {
            NUM_CLUST = UninitializedInt(args["NUM_CLUST"]);
        }
        catch (std::exception e)
        {
            // No problem, will be set later
        }
    }
    int num_choices()
    {
        return this->NUM_POINTS.get();
    };
    std::string get_key();
    std::vector<int> get_values();
    std::map<std::string, int> get_constants();
    void print();
};
class TSPConfig : public AbstractConfig
{
public:
    UninitializedInt SEED_GLOB,
        SEED_PROBLEM,
        SEED_ASSIGN,
        DATASET,
        FI_BI,
        IMPR,
        NUM_TOWNS,
        NUM_DIM;

public:
    TSPConfig(std::map<std::string, int> args)
    {
        SEED_GLOB = UninitializedInt(args["SEED_GLOB"]);
        SEED_PROBLEM = UninitializedInt(args["SEED_PROBLEM"]);
        SEED_ASSIGN = UninitializedInt(args["SEED_ASSIGN"]);
        DATASET = UninitializedInt(args["DATASET"]);
        FI_BI = UninitializedInt(args["FI_BI"]);
        IMPR = UninitializedInt(args["IMPR"]);
        try
        {
            NUM_TOWNS = UninitializedInt(args["NUM_TOWNS"]);
        }
        catch (std::exception e)
        {
            // No problem, will be set later
        }
        try
        {
            NUM_DIM = UninitializedInt(args["NUM_DIM"]);
        }
        catch (std::exception e)
        {
            // No problem, will be set later
        }
    }
    int num_choices()
    {
        return this->NUM_TOWNS.get();
    }
    std::string get_key();
    std::vector<int> get_values();
    std::map<std::string, int> get_constants();
    void print();
};
class MAXSATConfig : public AbstractConfig
{
public:
    UninitializedInt SEED_GLOB,
        SEED_PROBLEM,
        SEED_ASSIGN,
        DATASET,
        FI_BI,
        IMPR,
        NUM_VARIABLES,
        NUM_CLAUSES;
    MAXSATConfig(std::map<std::string, int> args)
    {
        SEED_GLOB = UninitializedInt(args["SEED_GLOB"]);
        SEED_PROBLEM = UninitializedInt(args["SEED_PROBLEM"]);
        SEED_ASSIGN = UninitializedInt(args["SEED_ASSIGN"]);
        DATASET = UninitializedInt(args["DATASET"]);
        FI_BI = UninitializedInt(args["FI_BI"]);
        IMPR = UninitializedInt(args["IMPR"]);
        try
        {
            NUM_VARIABLES = UninitializedInt(args["NUM_VARIABLES"]);
        }
        catch (std::exception e)
        {
            // No problem, will be set later
        }
        try
        {
            NUM_CLAUSES = UninitializedInt(args["NUM_CLAUSES"]);
        }
        catch (std::exception e)
        {
            // No problem, will be set later
        }
    }
    int num_choices()
    {
        return this->NUM_VARIABLES.get();
    }
    std::string get_key();
    std::vector<int> get_values();
    std::map<std::string, int> get_constants();
    void print();
};
#endif
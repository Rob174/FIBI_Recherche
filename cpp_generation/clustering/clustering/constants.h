#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>
#include <iostream>
#include <vector>
class Config
{
public:
    int NUM_POINTS;//
    int NUM_DIM;//
    int POINTS_DISTR;//
    int NUM_CLUST;//
    int CLUST_IMPR;//
    int IMPR_CLASS;//
    int IT_ORDER;//
    long SEED_POINTS;//
    long SEED_ASSIGN;//
    long SEED_GLOB;
    int ID;
    void print()
    {
        std::cout
            << "SEED_GLOB:" << SEED_GLOB << ","
            << "NUM_POINTS:" << NUM_POINTS << ","
            << "POINTS_DISTR:" << POINTS_DISTR << ","
            << "NUM_CLUST:" << NUM_CLUST << ","
            << "CLUST_IMPR:" << CLUST_IMPR << ","
            << "IMPR_CLASS:" << IMPR_CLASS << ","
            << "IT_ORDER:" << IT_ORDER << ","
            << "SEED_POINTS:" << SEED_POINTS << ","
            << "SEED_ASSIGN:" << SEED_ASSIGN << ","
            << "ID:" << ID << std::endl;
    }
    std::vector<int>* get_values() {
        std::vector<int>* values = new std::vector<int>[9]; 
        values->push_back((int)SEED_GLOB);
        values->push_back((int)NUM_POINTS);
        values->push_back((int)POINTS_DISTR);
        values->push_back((int)NUM_CLUST);
        values->push_back((int)CLUST_IMPR);
        values->push_back((int)IMPR_CLASS);
        values->push_back((int)IT_ORDER);
        values->push_back((int)SEED_POINTS);
        values->push_back((int)SEED_ASSIGN);
        return values;
    }
};

#endif
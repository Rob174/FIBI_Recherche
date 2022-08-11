#pragma once
#include "constants.h"
#include <iostream>
#include <string>
class IterationOrder
{
protected:
    int i; // count the number of point iterated through
    int curr_clust_id;
    Config* config;

public:
    IterationOrder(Config* config) : i(0), curr_clust_id(0), config(config) {};
    ~IterationOrder() {};
    virtual void restart(int curr_clust_id);
    virtual void end_loop() {};
    virtual int next() = 0;
};
class BACK : public IterationOrder
{
public:
    BACK(Config* config) : IterationOrder(config) {};
    ~BACK() {};
    int next_id();
    int next();
};
class CURR : public IterationOrder
{
public:
    CURR(Config* config) : IterationOrder(config) {};
    ~CURR() {};
    int next();
};
class RANDOM : public IterationOrder
{
private:
    int* random_permutation(int n);
    int* perm;

public:
    RANDOM(Config* config) : IterationOrder(config) {};
    ~RANDOM() {
        delete[] perm;
    }
    void restart(int curr_clust_id);
    void end_loop();
    int next_id();
    int next();
};

#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>

#include "exceptions.h"

class AbstractConfig
{
protected:
    std::map<const char *, int> *constants;

public:
    AbstractConfig(std::map<const char *, int> *args);
    void print();
    std::vector<int> get_values();
    virtual bool checkArgsValid(bool blocking = true);
    int get(const char *name);
};
class ClusteringConfig : public AbstractConfig
{

public:
    bool checkArgsValid(bool blocking = true);
};
class TSPConfig : public AbstractConfig
{

public:
    bool checkArgsValid(bool blocking = true);
};

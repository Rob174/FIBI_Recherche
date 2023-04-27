#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__
#include <exception>
#include <string>
#include "utils.h"

class KeyNotFound : public std::exception
{
private:
    const char *name;

public:
    KeyNotFound(const char *name)
    {
        this->name = name;
    }
    const char *what()
    {
        return format<const char *>("Cannot find key %s", this->name).c_str();
    }
};

#endif // __EXCEPTIONS_H__
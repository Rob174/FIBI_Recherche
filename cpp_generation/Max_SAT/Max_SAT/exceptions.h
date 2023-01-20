#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__
#include <exception>
#include <string>
#include "utils.h"

#define THROW_EXC1(arg1,code) throw GenericException(std::vector<std::string>{arg1},code)
#define THROW_EXC2(arg1,arg2,code) throw GenericException(std::vector<std::string>{arg1,arg2},code)
#define THROW_EXC3(arg1,arg2,arg3,code) throw GenericException(std::vector<std::string>{arg1,arg2,arg3},code)
#define THROW_EXC3_WARNING(arg1,arg2,arg3,code) std::cout << arg1 << arg2 << arg3 << std::endl;
#define THROW_EXC4(arg1,arg2,arg3,arg4,code) throw GenericException(std::vector<std::string>{arg1,arg2,arg3,arg4},code)
#define THROW_EXC4_WARNING(arg1,arg2,arg3,arg4,code) std::cout << arg1 << arg2 << arg3 << arg4 << std::endl;
#define THROW_EXC5(arg1,arg2,arg3,arg4,arg5,code) throw GenericException(std::vector<std::string>{arg1,arg2,arg3,arg4,arg5},code)
#define THROW_EXC6(arg1,arg2,arg3,arg4,arg5,arg6,code) throw GenericException(std::vector<std::string>{arg1,arg2,arg3,arg4,arg5,arg6},code)
#define THROW_EXC9(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,code) throw GenericException(std::vector<std::string>{arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9},code)
class KeyNotFound : public std::exception
{
private:
    std::string name;
    std::string msg;
public:
    KeyNotFound(std::string name)
    {
        this->name = name;
        msg = join(std::vector<std::string> {"Cannot find key ", this->name});
    }
    const char *what()
    {
        return msg.c_str();
    }
};
class TooManyElements : public std::exception
{
private:
    const int expected;
    const int found;
    std::string msg;

public:
    TooManyElements(const int expected, const int found) : expected(expected), found(found)
    {
        msg = join(std::vector<std::string> {"Too many keys found with expected ", std::to_string(expected), " but found ", std::to_string(found)});
    }
    const char* what()
    {
        return msg.c_str();
    }
};

class TooLittleElements : public std::exception
{
private:
    std::string name;
    const int number_min;
    std::string msg;
public:
    TooLittleElements(const char* name, const int number_min) : name(name), number_min(number_min)
    {
        msg = join(std::vector<std::string> {"Too little arguments provided for ", this->name, " expected at least ", std::to_string(this->number_min)});
    }
    const char* what()
    {
        return msg.c_str();
    }
};
class GenericException : public ::std::exception {
    std::vector<std::string> args;
    std::string msg;
    const int code;
public:
    GenericException(std::vector<std::string> args, const int code = 0) : args(args),code(code)
    {
        msg = join(this->args);
    }
    int get_code() { return code; }
    std::string get_msg() { return msg; }
    const char* what()
    {
        return msg.c_str();
    }
};
#endif // __EXCEPTIONS_H__
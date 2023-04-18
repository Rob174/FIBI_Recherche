#include "utils.h"
template <typename... T>
std::string format(std::string s, T... args)
{
    int num_char;
    num_char = snprintf(NULL, 0, s, ... args) + 1;
    char *buffer = new char[num_char * sizeof(char)];
    snprintf(buffer, num_char, s, ... args);
    return std::string(buffer);
}

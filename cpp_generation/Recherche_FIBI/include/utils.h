#ifndef __UTILS_H__
#define __UTILS_H__
#include <string>

template <typename ...T>
std::string format(std::string s, T... args);
#endif // __UTILS_H__
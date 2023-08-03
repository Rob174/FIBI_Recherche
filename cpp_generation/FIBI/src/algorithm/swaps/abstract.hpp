#pragma once
#include <map>
#include <string>
using namespace std;
class SwapChoice
{
public:
	SwapChoice() {};
	virtual std::map<std::string,long> get_dict(){return {};};
};
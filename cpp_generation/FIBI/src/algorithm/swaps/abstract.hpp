#pragma once
#include <map>
using namespace std;
class SwapChoice
{
public:
	SwapChoice() {};
	virtual map<string,long> get_dict() = 0;
};
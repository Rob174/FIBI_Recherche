#pragma once
#include <map>
using namespace std;
class SwapChoice
{
public:
	SwapChoice() {};
	virtual map<string, int> get_dict() = 0;
};
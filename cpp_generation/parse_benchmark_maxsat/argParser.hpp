#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <sstream>

template <typename... Args>
void error_msg(const Args&... args) {
	std::stringstream error_msg;
	(error_msg << ... << args);
	throw std::runtime_error(error_msg.str());
}
// https://stackoverflow.com/questions/865668/parsing-command-line-arguments-in-c
char* getCmdOption(char** begin, char** end, const std::string& option)
{
	char** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	error_msg<std::string>("Flag '", option, "' has not been found");
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}
#define P_FLAG_SEEN(flag) cmdOptionExists(argv, argv + argc, flag)
#define P_GET_VAL(flag) getCmdOption(argv, argv + argc, flag)
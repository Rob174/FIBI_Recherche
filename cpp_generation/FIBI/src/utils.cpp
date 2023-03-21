#include "utils.h"

std::string to_string(const char* v)
{
	return std::string(v);
}
int binomial_coeff(int k, int n) {
	if (k < 0 || k > n) {
		return 0;
	}
	if (k == 0 || k == n) {
		return 1;
	}
	int res = 1;
	for (int i = 1; i <= k; ++i) {
		res *= n - k + i;
		res /= i;
	}
	return res;
}
std::string join(std::vector<std::string> texts, std::string separator)
{
	bool start = true;
	std::string out = "";
	for (std::string s : texts)
	{
		if (!start)
		{
			out.append(separator);
		}
		out.append(s);
		start = false;
	}
	return out;
}

std::wstring to_wstring_p(std::string s)
{
	return std::wstring(s.begin(), s.end());
}

#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iostream>

#include "./get_data.hpp"

using namespace std;
#define TEST_MACRO(name) \
    void name()          \
    {
#define REGISTER(fn)                 \
    functions_to_run.push_back(&fn); \
    functions_names.push_back(#fn);
void run_tests()
{
    vector<void (*)()> functions_to_run;
    vector<string> functions_names;
    REGISTER(test_uniform_points);
    REGISTER(test_normal_points);
    REGISTER(test_random_clust);

    for (int i = 0; i < functions_names.size(); i++)
    {
        try
        {
            functions_to_run.at(i)();
            cout << "\x1B[32m " << functions_names.at(i) << ":OK \033[0m " << endl;
        }
        // catch all runtime errors
        catch (const runtime_error &e)
        {
            cout << "\x1B[31m " << functions_names.at(i) << ":FAIL \033[0m " << endl;
            cout << e.what() << endl;
        }
    }
}
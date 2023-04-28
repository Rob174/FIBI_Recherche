// FIBI.cpp : Defines the entry point for the application.
//
/*
#include "FIBI.h"

using namespace std;

int main(const int argc, char** argv)
{
	run_tests(argc, argv);
	return 0;
}
*/
#include <random>
#include <iostream>

int main()
{
    std::mt19937 gen(12345);
    std::uniform_int_distribution<> dis(0, 10);

    int rand_num = dis(gen);

    std::cout << "Random number: " << rand_num << std::endl;
    return 0;
}
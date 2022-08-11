#include "tests.h"
#include "utils.h"
#include <set>
#include <chrono>
#include <thread>
#include <iostream>
#include <array>
#include<sstream>
#include <string>
//0 : pass ; 1 : fail
void print(std::string name) {
	std::cout << "Test " << name<<": ";
}
void succ_result() {
	std::cout << "\x1B[32mPASS\033[0m" << std::endl;
}
void run_tests() {
	std::vector<int (*)()> test_functions = { 
	};
	for (auto& f : test_functions) {
		if (f() != 0)
			break;
	}
}
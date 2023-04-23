#pragma once
#include <unordered_map>
#include <map>
#include "../../src/factories/maxsat.hpp"
#include "../../src/data/constants/maxsat.hpp"
using namespace std;
void test_MAXSATFactory() {
	const string root_data = "../../../data/folder/";
	string out_data = "../../../data/out/";
	int i = 0;
	for (int dataset = 0; dataset < 2; dataset++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 2; impr++) {
				MAXSATFactory f;
				map<string,long> *args = new map<string,long>{
					{"DATASET",dataset},
					{"SEED_GLOB",i},
					{"SEED_PROBLEM",0},
					{"SEED_ASSIGN",0},
					{"FI_BI",FI_BI},
					{"IMPR",impr},
					{"NUM_VARIABLES",100},
					{"NUM_CLAUSES",50}
				};
				MAXSATConfig cf(args);
				f.run(cf, root_data);
				cout << "\x1B[32m \tOK ";
				cf.print();
				delete args;
				cout << "\033[0m " << endl;
				i++;
			}
		}
	}
}
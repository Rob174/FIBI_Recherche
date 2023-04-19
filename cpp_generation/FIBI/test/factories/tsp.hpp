#pragma once
#include <unordered_map>
#include <map>
#include "../../src/factories/tsp.hpp"
#include "../../src/data/constants/tsp.hpp"
using namespace std;
void test_TSPFactory() {
	string root_data = "../../../data/folder/";
	string out_data = "../../../data/out/";
	TSPFactory f;
	int i = 0;
	for (int dataset = 0; dataset < 3; dataset++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 2; impr++) {
				map<string, int> args{
					{"DATASET",dataset},
					{"SEED_GLOB",i},
					{"SEED_PROBLEM",0},
					{"SEED_ASSIGN",0},
					{"FI_BI",FI_BI},
					{"IMPR",impr},
					{"NUM_TOWNS",20},
					{"NUM_DIM",2}
				};
				TSPConfig cf(args);
				f.run(cf, root_data);

				cout << "\x1B[32m \tOK ";
				cf.print();
				cout << "\033[0m " << endl;
				i++;
			}
		}
	}
}
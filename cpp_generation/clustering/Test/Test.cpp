#include "pch.h"
#include "CppUnitTest.h"
#include "../clustering/generate_instances.h"
#include "../clustering/generate_instances.cpp"
#include <random>
#include <cstdio>
#include <math.h>
#include <vector>
#include <sstream>
#include <iterator>

#define PRECISION 1e-6
#define NUM_REP 10

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<typename... Args>
const wchar_t* format(std::string fmt1, Args ...a) {
	const char* fmt = fmt1.c_str();
	int sz = std::snprintf(nullptr, 0, fmt, a...);
	std::vector<char> buf(sz + 1); // note +1 for null terminator
	std::snprintf(&buf[0], buf.size(), fmt, a...);
	std::wstring msg(buf.begin(), buf.end());
	return msg.c_str();
}
template<typename T>
std::string vec_to_string(std::vector<T> vec) {
	std::stringstream ss;

	for (size_t i = 0; i < vec.size(); i++) {
		if (i != 0) {
			ss << ", ";
		}
		ss << vec[i];
	}
	return ss.str();
}
template<typename T>
bool equal(T v1, T v2) {
	return abs(v1 - v2) < PRECISION;
}

namespace Tests
{
	TEST_CLASS(TestsModifiedBenchmark)
	{
	public:

		TEST_METHOD(LinspaceStartEnd)
		{
			const double MAX = 1000;

			std::mt19937 gen;
			std::uniform_int_distribution<> d_num{ 2,1000 };
			std::uniform_real_distribution<double> d_min{ -MAX,0 };
			std::uniform_real_distribution<double> d_max{ 0,MAX };

			const int num = d_num(gen);
			const double start = d_min(gen);
			const double end = d_max(gen);

			double* p = linspace_p(start, end, num);


			Assert::IsTrue(start == p[0], format<double>("Start must be %f. Found %f",start,p[0]));
			Assert::IsTrue(equal<double>(end,p[num-1]), format<double>("End must be %f. Found %f", end, p[num-1]));

			delete[] p;
		}
		TEST_METHOD(LinspaceBackward)
		{
			const double MAX = 1000;

			std::mt19937 gen;
			std::uniform_int_distribution<> d_num{ 2,1000 };
			std::uniform_real_distribution<double> d_min{ 0,MAX };
			std::uniform_real_distribution<double> d_max{ -MAX,0 };

			const int num = d_num(gen);
			const double start = d_min(gen);
			const double end = d_max(gen);

			double* p = linspace_p(start, end, num);


			Assert::IsTrue(start == p[0], format<double>("Start must be %f. Found %f", start, p[0]));
			Assert::IsTrue(equal<double>(end, p[num - 1]), format<double>("End must be %f. Found %f", end, p[num - 1]));

			delete[] p;
		}
		TEST_METHOD(generate_gaussianNotEmpty)
		{
			Config conf;
			std::mt19937 gen;
			std::uniform_int_distribution<> d_dim{ 2,1000 };
			std::uniform_int_distribution<> d_seed{ 0,1000 };
			std::uniform_real_distribution<double> d_center{ 0,1000000 };
			std::uniform_int_distribution<> d_n_pts{ 0,100000 };
			std::uniform_real_distribution<> d_stds{ 0,1000 };
			for (int i = 0; i < NUM_REP; i++) {
				conf.NUM_DIM = d_dim(gen);
				conf.SEED_POINTS = d_seed(gen);
				double* center = new double[conf.NUM_DIM];
				for (int j = 0; j < conf.NUM_DIM; j++) {
					center[j] = d_center(gen);
				}
				const int num_points = d_n_pts(gen);
				double* stds = new double[conf.NUM_DIM];
				for (int j = 0; j < conf.NUM_DIM; j++) {
					stds[j] = d_stds(gen);
				}
				double* p = generate_gaussian(center, num_points, stds, &conf);
				std::vector<int> null_values{};
				for (int j = 0; j < conf.NUM_POINTS; j++) {
					for (int k = 0; k < conf.NUM_DIM; k++) {
						if (abs(p[j * conf.NUM_DIM + k]) < PRECISION) {
							null_values.push_back(j);
						}
					}
				}
				std::string null_vals = vec_to_string<int>(null_values);
				Assert::IsTrue(
					null_vals.size() == 0, 
					format<std::string>("All values must be initialized. Found %s null elements on %s elements at points %s", std::to_string(null_values.size()),std::to_string(num_points),null_vals)
				);
				delete[] center;
				delete[] stds;
				delete[] p;
			}
		}
	};
}

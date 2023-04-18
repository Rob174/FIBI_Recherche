#include "pch.h"
#include "CppUnitTest.h"

#include "utils.h"
#include "utils.cpp"
#include "constants.h"
#include "constants.cpp"
#include <string>
#include "exceptions.h"
#include "exceptions.cpp"
#include "SolutionSaver.h"
#include "SolutionSaver.cpp"
#include "Container.h"
#include "Container.cpp"
#include <random>
#include <algorithm>

#include "MAXSATgenerator.h"
#include "MAXSATgenerator.cpp"
#include "Algorithm.h"
#include "Algorithm.cpp"
#include "AlgorithmObservable.h"
#include "AlgorithmObservable.cpp"
#include "AlgorithmObserver.h"
#include "AlgorithmObserver.cpp"
#include "result.h"
#include "result.cpp"
#include "Metrics.h"
#include "Metrics.cpp"
#include "H5Cpp.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	
	TEST_CLASS(TestConfig)
	{
	public:
		TEST_METHOD(MissingKey)
		{
			bool exception = false;
			std::map<std::string, int> args;
			args.insert(std::make_pair(to_string("NUM_VARIABLES"), 20));
			MAXSATConfig conf(&args);
			try {
				conf.check_keys_before_load_instance();
			}
			catch (KeyNotFound &e) {
				exception = true;
			}
			Assert::IsTrue(exception, 
				to_wstring_p(join(
					std::vector<std::string>{"Expected KeyMissing", std::string("")}
			)).c_str()
				);
		}
		TEST_METHOD(KeyExcess)
		{
			std::map<std::string, int> args;
			args.insert(std::make_pair(to_string("SEED_GLOB"), 20));
			args.insert(std::make_pair(to_string("NUM_VARIABLES"), 20));
			args.insert(std::make_pair(to_string("SEED_CLAUSE"), 2));
			args.insert(std::make_pair(to_string("I_DONT_KNOW_WHAT_TO_SAY"), 3));
			MAXSATConfig conf(&args);
			bool raise_exception = false;//Variable required!!! Cannot use directly boolean
			try {
				conf.check_keys_before_load_instance();
				raise_exception = true;
				Assert::IsTrue(raise_exception,
					to_wstring_p(join(
						std::vector<std::string>{"Expected GenericException"}
				)).c_str());
			}
			catch (KeyNotFound& e) {
				raise_exception = true;
				Assert::IsTrue(raise_exception,
					to_wstring_p(join(
						std::vector<std::string>{
					"Unexpected KeyNotFound exception. Review test arguments provided. Missing key",
						std::string(e.what())
				})).c_str());
			}
			catch (std::exception& e) {
				raise_exception = true;
				Assert::IsTrue(raise_exception,
					to_wstring_p(join(
						std::vector<std::string>{
					"Unexpected exception",
						std::string(e.what())
				})).c_str());
			}
		}
	};
	
	TEST_CLASS(TestMAXSATgenerate_random_clauses)
	{
	public:

		TEST_METHOD(SimpleUsage)
		{
			for (int i = 0; i < 100; i++) {
				MAXSATContainer*c = generate_random_clauses(3);
				delete c;
			}
		}
		TEST_METHOD(TooLongClause)
		{
			bool exception_triggered = false;
			try {
				MAXSATContainer* c = generate_random_clauses(3, 10);
				delete c;
			}
			catch (GenericException e) {
				exception_triggered = true;
			}
			Assert::IsTrue(exception_triggered, to_wstring_p("Expected GenericException because of too many clauses").c_str());
		}
		TEST_METHOD(CheckNumVarRand)
		{
			std::uniform_int_distribution rd_num_var(10,1000);
			std::random_device rd;
			std::mt19937 mt(rd());
			bool correct_num_var = false;
			for (int i = 0; i < 10; i++) {
				const int num = rd_num_var(mt);
				MAXSATContainer* c;
				try {
					 c = generate_random_clauses(num);
					 correct_num_var = c->get_num_variables() == num;
					 Assert::IsTrue(correct_num_var, to_wstring_p("Incorrect number of variables").c_str());
					 delete c;
				}
				catch (...) {
					Assert::IsTrue(correct_num_var);

				}
			}
		}
		TEST_METHOD(CheckTooSmallNumVar)
		{
#define SAFE_EXEC
			const int num = 0;
			bool exception_raised = false;
			try {
				MAXSATContainer* c = generate_random_clauses(num);
			}
			catch (TooLittleElements& e) {
				exception_raised = true;
			}
			catch (std::exception& e) {
				Assert::IsTrue(exception_raised, to_wstring_p(join(std::vector<std::string>{"Unexpected exception",std::string(e.what())})).c_str());
			}
			Assert::IsTrue(exception_raised, to_wstring_p("Expected TooLittleElements exception").c_str());
		}
	};
	
	TEST_CLASS(TestMAXSATSolutionSaver)
	{
	public:

		TEST_METHOD(NoProblem)
		{
			SolutionSaver* saver = new SolutionSaver();
			for (int i = 0; i < 1000; i++) {
				MAXSATContainer* cont = generate_random_clauses(3);
				MAXSATImage* img = new MAXSATImage(i, cont);
				delete cont;
				saver->add_container(img);
			}
			delete saver;
			delete MAXSATImage::clauses_img;
		}
	};
	TEST_CLASS(TestMAXSATAlgorithm)
	{
	public:
		TEST_METHOD(NoProblemFI)
		{
			for (bool FI : {true, false}) {
#define DEBUG_LV1
				std::map<std::string, int>* args = new std::map<std::string, int>();
				args->insert(std::make_pair("NUM_VARIABLES", 3));
				args->insert(std::make_pair("CLAUSE_SIZE", -1));
				args->insert(std::make_pair("CLAUSE_SIZE_GENMODE", 0));
				MAXSATContainer* cont = generate_random_clauses(args->at("NUM_VARIABLES"));
				cont->print();
				MAXSATConfig* conf = new MAXSATConfig(args);
				Metrics* metrics = new Metrics();
				MAXSATResult* res = new MAXSATResult(conf,metrics);
				MAXSATAlgorithm* alg = new MAXSATAlgorithm(std::vector< MAXSATAlgoObs*>{res});
				alg->run(cont, conf, FI);
				delete cont;
				delete res;
				delete alg;
				delete conf;
				delete args;
				delete metrics;
			}
		}
	};
	
}
#ifndef SOLUTIONSAVER_H
#define SOLUTIONSAVER_H
#include "AlgorithmObserver.h"
#include "AbstractWritable.h"
#include "Container.h"
#include <iostream>

class AbstractContainerImage
{
protected:
	int step;
	std::vector<double> image;

public:
	AbstractContainerImage(int step) : step(step){};
	std::vector<double> get_image();
};
class MAXSATImage : public AbstractContainerImage
{
private:
	const int num_variables;

public:
	static std::vector<double> *clauses_img;
	MAXSATImage(int step, MAXSATContainer *cont);
	std::vector<double> get_image();
};
template <typename T_Container, typename T_Swap>
class SolutionSaver : AlgorithmObserver< T_Container, T_Swap>, AbstractWritable<double>
{
public:
	std::vector<AbstractContainerImage *> elems;

public:
	void add_container(AbstractContainerImage *cont)
	{
		this->elems.push_back(cont);
	};
	~SolutionSaver() {

		while (!elems.empty()) {
			AbstractContainerImage* p = elems.back();
			delete p;
			elems.pop_back();
		}
	}

	std::vector<std::string> get_group()
	{
		return std::vector<std::string>{"solutions", "metadata_solutions"};
	};
	std::vector<std::vector<double>> get_data()
	{
		std::vector<std::vector<double>> data;
		for (auto& e : this->elems)
		{
			data.push_back(e->get_image());
		}
		return data;
	};
	void print()
	{
		int i = 0;
		for (auto& e : this->get_data())
		{
			std::cout << "--------------Step " << i << " --------------------" << std::endl;
			for (auto& a : e)
			{
				std::cout << a;
			}
			std::cout << std::endl;
			i++;
		}
	};
};
#endif
#include "SolutionSaver.h"

std::vector<double>* MAXSATImage::clauses_img = new std::vector<double>();
MAXSATImage::MAXSATImage(int step, MAXSATContainer *cont) : AbstractContainerImage(step), num_variables(cont->get_num_variables())
{
	// an image will be stored in the form
	// [size_tot, num_var, objective, w1, ..., wn, val_var1, ..., val_varn, num_clause, size_clause1, clause1_var, clause1_neg, ... clausen_var, clausen_neg]
	int total_size = 2;
	
	if (MAXSATImage::clauses_img->size() == 0)
	{
		int clause_tot_size = 1;
		for (clause_t c : cont->get_clauses())
		{
			clause_tot_size += c.size() * 2 + 1; // *2 because variable index + negation +1 because indicate number of variables in clause
		}
		MAXSATImage::clauses_img->push_back(cont->get_clauses().size());
		for (clause_t c : cont->get_clauses())
		{
			MAXSATImage::clauses_img->push_back(c.size());
			for (clause_elem_t e : c)
			{
				MAXSATImage::clauses_img->push_back(e.first);
				MAXSATImage::clauses_img->push_back(e.second);
			}
		}
	}
	std::vector<double> image{(double)cont->get_num_variables()};

	const double *weights_cont = cont->get_weights();
	const bool *val_var_cont = cont->get_variables_val();
	int j = 2;
	for (int i = 0; i < cont->get_num_variables(); i++)
	{
		image.push_back(weights_cont[i]);
	}
	delete[] weights_cont;
	for (int i = 0; i < cont->get_num_variables(); i++)
	{
		image.push_back(val_var_cont[i] ? 1 : 0);
	}
	image.insert(image.end(), MAXSATImage::clauses_img->begin(), MAXSATImage::clauses_img->end());
}
std::vector<double> AbstractContainerImage::get_image()
{
	return image;
}

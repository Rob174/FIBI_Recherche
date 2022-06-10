#include "improvementChoice.h"


bool BestImpr::stop_loop()
{
	return false;
}

bool FirstImpr::stop_loop()
{
	return true;
}


void ImprovementChoice::choose_solution(ClusteringChoice*choice, Clustering* clustering, double sugg_vij, int sugg_i, int sugg_l, int sugg_j)
{
    choice->vij = sugg_vij;
    choice->i = sugg_i;
    choice->l = sugg_l;
    choice->j = sugg_j;
}

bool ImprovementChoice::stop_loop()
{
    return false;
}
void ImprovementChoice::after_choice()
{
    return;
}
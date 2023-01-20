#include "factory_observer.h"

void AbstractMAXSATFactoryObservable::on_setup_end(MAXSATContainer *cont, MAXSATResult *res)
{
	for (AbstractMAXSATFactoryObserver *o : this->obs)
	{
		o->on_setup_end(cont, res);
	}
}
void AbstractClusteringFactoryObservable::on_setup_end(ClusteringContainer *cont, ClusteringResult*res)
{
	for (AbstractClusteringFactoryObserver *o : this->obs)
	{
		o->on_setup_end(cont, res);
	}
}
void AbstractTSPFactoryObservable::on_setup_end(TSPContainer *cont, TSPResult *res)
{
	for (AbstractTSPFactoryObserver *o : this->obs)
	{
		o->on_setup_end(cont, res);
	}
}

void ClausesChecker::on_setup_end(MAXSATContainer *cont, MAXSATResult*res)
{
	std::vector<clause_t> clauses = cont->get_clauses();
	for (clause_t &c_found : clauses)
	{
		bool found_clause = false;
		int num_found_var = 0;
		for (clause_t &c_exp : this->expected_clauses)
		{
			if (c_exp.size() != c_found.size())
				continue;
			for (clause_elem_t &e_found : c_found)
			{
				bool found_var = false;
				for (clause_elem_t &e_exp : c_exp)
				{
					if (e_found.first == e_exp.first && e_found.second == e_exp.second)
					{
						found_var = true;
						num_found_var++;
						break;
					}
				}
				if (found_var == false)
					break;
			}
		}
		found_clause = num_found_var == c_found.size();
		if (found_clause == false)
		{
			THROW_EXC4(
				"Non-matching clauses with found ",
				MAXSATContainer::clauses_str(clauses),
				" and expected ",
				MAXSATContainer::clauses_str(this->expected_clauses),
				12);
		}
	}
}
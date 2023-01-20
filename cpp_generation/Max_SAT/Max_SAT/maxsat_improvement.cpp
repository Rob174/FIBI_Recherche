#include "maxsat_improvement.h"
#define LOG() std::cout << "LOG " << __FILE__ << ":" << std::to_string(__LINE__) << std::endl;
#define LOG1(s1) std::cout << s1 << std::endl;
#define LOG2(s1, s2) std::cout << s1 << " | " << s2 << std::endl;
// #define DEBUG_IMPR
#define SHOW_STAT(name, stat_struct) name << "SAT:" << stat_struct.SAT << " ; UNSAT:" << stat_struct.UNSAT << " ; B:" << stat_struct.B
#define SHOW_STEP(step_name, value, stat_struct1, stat_struct2) step_name << ": " << value << std::endl                                     \
                                                                          << "    " << SHOW_STAT("former_stats", stat_struct1) << std::endl \
                                                                          << "    " << SHOW_STAT("curr_stats", stat_struct2) << std::endl;
void improve(MAXSATContainer *cont, const int seed)
{
    std::mt19937 gen{(const unsigned int)seed};
    // Taken from paper Bounds on Greedy Algorithms for MAX SAT https://link.springer.com/content/pdf/10.1007/978-3-642-23719-5.pdf
    // with explanations of https://diglib.tugraz.at/download.php?id=5b6d299d67215&location=browse
    // main source https://people.orie.cornell.edu/mup3/Poloczek%20Schnitger%20Williamson%20van%20Zuylen%20-%20Greedy%20Algorithms%20for%20MAX%20SAT__SICOMP_2017.pdf p1038 and 1058-59
    // Slack algorithm
    std::map<const var_id_t, std::vector<clause_id_t>> var_in_clauses = cont->get_var_in_clauses();
    std::vector<std::vector<std::set<clause_id_t>>> undecided_clauses_per_var = {};
    for (var_id_t var_id = 0; var_id < cont->get_num_variables(); var_id++)
    {
        std::vector<std::set<clause_id_t>> undecided = {std::set<clause_id_t>{}, std::set<clause_id_t>{}};
        undecided_clauses_per_var.push_back(undecided);
    }
    std::vector<int> number_unfixed_literals;
    std::vector<clause_t> clauses = cont->get_clauses();
    for (clause_id_t i = 0; i < clauses.size(); i++)
    {
        auto &clause = clauses.at(i);
        number_unfixed_literals.push_back(clause.size());
        for (auto &e : clause)
        {
            undecided_clauses_per_var.at(e.first).at((int)(e.second)).insert(i);
        }
    }
    std::vector<var_id_t> var_id_order;
    for (int i = 0; i < cont->get_num_variables(); i++)
    {
        var_id_order.push_back(i);
    }
    std::shuffle(var_id_order.begin(), var_id_order.end(), gen);
    double weight_satisfied = 0;
    std::vector<double> weights;
    const double *weights_values = cont->get_pweights();
    double total_weights = 0;
    for (int i = 0; i < cont->get_clauses().size(); i++)
    {
        total_weights += weights_values[i];
    }
    stats formerStats = {.SAT = 0, .UNSAT = 0, .B = total_weights / 2.};
    auto cmptB = [total_weights](stats &s)
    { return 0.5 * (double)(s.SAT + (total_weights - s.UNSAT)); };
    for (var_id_t var_id : var_id_order)
    {
        // ti: if we set xi to true, all clauses with only one variable left
        stats currStatsTrue = {.SAT = formerStats.SAT, .UNSAT = formerStats.UNSAT, .B = formerStats.B};
        // and without a negation will be set to true
        for (int clause_id : undecided_clauses_per_var.at(var_id).at(1))
        {
            currStatsTrue.SAT += cont->get_pweights()[clause_id];
        }
        // and with a negation will be set to false not true if previously a true assignement (disjonction of literals)
        for (int clause_id : undecided_clauses_per_var.at(var_id).at(0))
        {
            if (number_unfixed_literals.at(clause_id) == 1)
            {
                currStatsTrue.UNSAT += cont->get_pweights()[clause_id];
            }
        }
        currStatsTrue.B = cmptB(currStatsTrue);
        const double ti = currStatsTrue.B - formerStats.B;
#ifdef DEBUG_IMPR
        std::cout << SHOW_STEP("ti", ti, formerStats, currStatsTrue);
#endif // DEBUG_IMPR

        // fi: if we set xi to false
        stats currStatsFalse = {.SAT = formerStats.SAT, .UNSAT = formerStats.UNSAT, .B = formerStats.B};
        for (int clause_id : undecided_clauses_per_var.at(var_id).at(1))
        {
            if (number_unfixed_literals.at(clause_id) == 1)
            {
                currStatsFalse.UNSAT += cont->get_pweights()[clause_id];
            }
        }
        for (int clause_id : undecided_clauses_per_var.at(var_id).at(0))
        {
            currStatsFalse.SAT += cont->get_pweights()[clause_id];
        }
        currStatsFalse.B = cmptB(currStatsFalse);
        const double fi = currStatsFalse.B - formerStats.B;
#ifdef DEBUG_IMPR
        std::cout << SHOW_STEP("fi", fi, formerStats, currStatsFalse);
#endif // DEBUG_IMPR
        bool value;
        if (fi <= 0)
        {
            // set to true
            value = true;
        }
        else if (ti <= 0)
        {
            // set to false
            value = false;
        }
        else
        {
            // set to true with probability ti/(ti+fi)
            std::uniform_real_distribution<double> dist(0, 1);
            value = dist(gen) < ti / (ti + fi);
        }
        cont->set_var_value(var_id, value);
        // Remove satisfied clauses and update stats
        // if false Lxi_bar if true Lxi to iterate
        const int idx_neg_sat = (int)(value);
        auto undecided = undecided_clauses_per_var.at(var_id).at(idx_neg_sat);

        for (int clause_id : std::vector<int>(undecided.begin(), undecided.end()))
        {
            undecided_clauses_per_var.at(var_id).at(idx_neg_sat).erase(clause_id);
            formerStats.SAT += cont->get_pweights()[clause_id];
        }
        // Decrement unfixed literals in other clauses
        const int idx_neg_unsat = (int)(!value);
        undecided = undecided_clauses_per_var.at(var_id).at(idx_neg_unsat);
        for (int clause_id : std::vector<int>(undecided.begin(), undecided.end()))
        {
            number_unfixed_literals.at(clause_id) -= 1;
            if (number_unfixed_literals.at(clause_id) == 0)
            {
                undecided_clauses_per_var.at(var_id).at(idx_neg_unsat).erase(clause_id);
                formerStats.UNSAT += cont->get_pweights()[clause_id];
            }
        }
        formerStats.B = cmptB(formerStats);
    }
}
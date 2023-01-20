#ifndef __RESULT_H__
#define __RESULT_H__
#include "Metrics.h"
#include "constants.h"
#include "Container.h"
#include "AlgorithmObserver.h"
#include "json.hpp"
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#define SYSERROR() GetLastError()
#else
#include <errno.h>
#define SYSERROR() errno
#endif
template <typename Swap_T>
class Result
{
protected:
    AbstractConfig *config;
    Metrics *metrics;

public:
    Result(AbstractConfig *config, Metrics *metrics) : config(config), metrics(metrics){};
    void start(AbstractContainer<Swap_T> *cont)
    {
        this->metrics->init_cost = cont->get_quality_metric();
        this->metrics->final_cost = cont->get_quality_metric();
        this->metrics->time_start = this->metrics->get_time();
    };
    void glob_iter_end(AbstractContainer<Swap_T> *container)
    {
        this->metrics->num_iter_glob++;
    };
    void iter_end(AbstractContainer<Swap_T> *container)
    {
        this->metrics->final_cost = container->get_quality_metric();
        this->metrics->num_iter++;
    };
    void end(AbstractContainer<Swap_T> *container)
    {
        this->metrics->final_cost = container->get_quality_metric();
        this->metrics->set_final_time();
    };

    std::vector<std::string> group()
    {
        return std::vector<std::string>{"metadata"};
    };
    std::vector<std::vector<double>> data()
    {
        std::vector<double> data;
        for (std::vector<double> values : this->config->get_data())
        {
            for (double v : values)
            {
                data.push_back(v);
            }
        }
        for (std::vector<double> values : this->metrics->get_data())
        {
            for (double v : values)
            {
                data.push_back(v);
            }
        }
        return std::vector<std::vector<double>>{data};
    };
    void print_result()
    {
        this->config->print();
        this->metrics->print();
    };
    void save_mapping()
    {
        std::vector<std::string> v1 = this->config->get_values_names();
        std::vector<std::string> v2 = this->metrics->get_data_names();
        v1.insert(v1.end(), v2.begin(), v2.end());

        using json = nlohmann::json;
        json j;
        for (int i = 0; i < v1.size(); i++)
        {
            j.push_back({{"id", std::to_string(i)}, {"name", v1[i]}});
        }
        std::ofstream o("mapping.json");
        if (!o)
        {
            std::cout << "Failed to open file : " << SYSERROR() << std::endl;
            THROW_EXC1("Cant open file mapping.json write", 501);
        }
        o << j.dump() << std::endl;
        o.close();
    };
    std::string key()
    {
        return this->config->get_key();
    };
};
class MAXSATResult : public AlgorithmObserver<MAXSATContainer, MAXSATSwap>, public AbstractWritable<double>, public Result<MAXSATSwap>
{
public:
    MAXSATResult(MAXSATConfig *config, Metrics *metrics) : AlgorithmObserver(), AbstractWritable<double>(), Result(config, metrics){};
    void on_start(MAXSATContainer *container) { return Result::start(container); };
    void on_glob_iter_end(MAXSATContainer *container) { return Result::glob_iter_end(container); };
    static MAXSATSwap _dummy_swap;
    void on_iter_end(MAXSATContainer *container, const MAXSATSwap &s = _dummy_swap) { return Result::iter_end(container); };
    void on_end(MAXSATContainer *container) { return Result::end(container); };

    std::vector<std::string> get_group() { return Result::group(); };
    std::vector<std::vector<double>> get_data() { return Result::data(); };
    void print() { return Result::print_result(); };
    std::string get_key() { return Result::key(); };
};
class ClusteringResult : public AlgorithmObserver<ClusteringContainer, ClusteringSwap>, public AbstractWritable<double>, public Result<ClusteringSwap>
{
public:
    ClusteringResult(ClusteringConfig *config, Metrics *metrics) : AlgorithmObserver(), AbstractWritable<double>(), Result(config, metrics){};
    void on_start(ClusteringContainer *container) { return Result::start(container); };
    void on_glob_iter_end(ClusteringContainer *container) { return Result::glob_iter_end(container); };
    static ClusteringSwap _dummy_swap;
    void on_iter_end(ClusteringContainer *container, const ClusteringSwap &s = _dummy_swap) { return Result::iter_end(container); };
    void on_end(ClusteringContainer *container) { return Result::end(container); };

    std::vector<std::string> get_group() { return Result::group(); };
    std::vector<std::vector<double>> get_data() { return Result::data(); };
    void print() { return Result::print_result(); };
    std::string get_key() { return Result::key(); };
};

class TSPResult : public AlgorithmObserver<TSPContainer, TSPSwap>, public AbstractWritable<double>, public Result<TSPSwap>
{
public:
    TSPResult(TSPConfig *config, Metrics *metrics) : AlgorithmObserver(), AbstractWritable<double>(), Result(config, metrics){};
    void on_start(TSPContainer *container) { return Result::start(container); };
    void on_glob_iter_end(TSPContainer *container) { return Result::glob_iter_end(container); };
    static TSPSwap _dummy_swap;
    void on_iter_end(TSPContainer *container, const TSPSwap &s = _dummy_swap) { return Result::iter_end(container); };
    void on_end(TSPContainer *container) { return Result::end(container); };

    std::vector<std::string> get_group() { return Result::group(); };
    std::vector<std::vector<double>> get_data() { return Result::data(); };
    void print() { return Result::print_result(); };
    std::string get_key() { return Result::key(); };
};
#endif // __RESULT_H__
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "tests.h"
#include "algorithm.h"
#include "clustering.h"
#include "utils.h"
void exec_non_diversified_dataset() {
    const int num_samples = 200000;
    const int start = 0;
    const int stop = num_samples * 2;
    for (int i = start; i < stop; i++) {
        auto args = random_argv(i, -1);
        int argc = std::get<0>(args);
        char** argv = std::get<1>(args);
        run(argc, argv, i, false, i / 2, i / 2);
        for (int a = 0; a < 7; a++)
            delete argv[a];
        delete argv;
    }
}
void exec_diversified_dataset() {
    long seed_points = 0;
    long seed_assigns = 0;
    int init = 0;
    const int num_diff_points_assigns = 1000;
    const int num_same_points = 10;
    for (int i = 0; i < 40 * num_diff_points_assigns; i++) {
        seed_assigns = i / 4;
        seed_points = i / (num_same_points * 4);
        init = (i % 4) / 2;
        auto args = random_argv(i, init);
        int argc = std::get<0>(args);
        char** argv = std::get<1>(args);
        run(argc, argv, i, false, seed_points, seed_assigns);
        for (int a = 0; a < 7; a++)
            delete argv[a];
        delete argv;
    }
}
int main(int argc, char *argv[])
{
    
    //  External run
    /*
    for (int i = 0; i < 1; i++)
        run(argc, argv,i, true,0,1);
    */
    exec_non_diversified_dataset();
    //Internal run,
    /*
    
    */
    
    //run_tests();
    return 0;
}
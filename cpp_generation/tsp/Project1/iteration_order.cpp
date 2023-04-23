#include "iteration_order.h"

void IterationOrder::restart(int curr_id, int n_prime)
{
    this->i = 0;
    this->curr_id = curr_id;
    this->n_prime = n_prime;
};
int BACK::next()
{
    int a = this->i+1;
    if (this->i >= this->config->NUM_POINTS)
        return -1;
    else if (a == this->curr_id || a == this->curr_id+1 || a == this->curr_id-1 )
    {
        this->i += 1;
        return next();
    }
    else
    {
        this->i++;
        return a;
    }
}
int CURR::next()
{
    int a = (this->i + this->curr_id + 2);
    if (a > this->n_prime)
    {
        return -1;
    }
    this->i++;
    return a;
}
int* RANDOM::random_permutation(int size)
{
    int* perm = new int[size];
    for (int i = 0; i < size; i++)
        perm[i] = i;
    for (int i = 0; i < size; i++)
    {
        int r = rand() % size;
        int tmp = perm[i];
        perm[i] = perm[r];
        perm[r] = tmp;
    }
    return perm;
}
void RANDOM::restart(int curr_id, int n_prime)
{
    IterationOrder::restart(curr_id, n_prime);
    this->perm = random_permutation(this->config->NUM_POINTS);
}
int RANDOM::next()
{
    if (this->i >= this->config->NUM_POINTS)
        return -1;
    int a = this->perm[this->i]+1;
    if (a == this->curr_id || a == (this->curr_id+1) || a == this->curr_id-1)
    {
        this->i++;
        return next();
    }
    else
    {
        this->i++;
        return a;
    }
}
void RANDOM::end_loop() {
    delete[] this->perm;
}
void IterationOrderFactory::print_doc()
{
    std::cout << "IterationOrder:BACK (0) CURR (1) RANDOM (2)" << std::endl;
}
IterationOrder* IterationOrderFactory::create(Config* config, int type)
{
    switch (type)
    {
    case 0:
        return new BACK(config);
    case 1:
        return new CURR(config);
    case 2:
        return new RANDOM(config);
    default:
        std::cout << "Wrong argument IterationOrder " << type << std::endl;
        exit(1);
    }
}
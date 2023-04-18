#include "tour.h"
//#define DEBUG
Tour::Tour(int* init_tour,Config* conf) {
    this->tour = init_tour;
    this->length = conf->NUM_POINTS;
}

Tour::~Tour() {
    delete[] this->tour;
}
double Tour::get_cost(DistanceMatrix *dist) {
    double tour_length = 0; 
    for (int i = 0; i < this->length;i++) {
        double d = dist->get(this->tour[i], this->tour[(i + 1) % this->length]);
        tour_length += d;
    }
    return tour_length;
}
double Tour::get_length(DistanceMatrix* dist) {
    double tour_length = 0;
    for (int i = 0; i < this->length; i++) {
        double d = dist->get(this->tour[i], this->tour[(i + 1) % this->length]);
        tour_length += d;
    }
    return tour_length;
}
int Tour::at(int i,Config*conf) {
    //town id at index i in the tour
    if (i >= conf->NUM_POINTS || i < 0) {
        std::cout << "Error id " << i << std::endl;
        exit(1);
    }
    return this->tour[i];
}

void Tour::apply_two_opt(int i, int j, Config*conf) {
    int buffer;
    if (i >= conf->NUM_POINTS || i < 0) {
        std::cout << "Wrong i with :" << i << std::endl;
        exit(1);
    }
    if (j >= conf->NUM_POINTS || j < 0) {
        std::cout << "Wrong j with :" << j << std::endl;
        exit(1);
    }
    if (cycle_id(i + 1, conf->NUM_POINTS) == j || cycle_id(j + 1, conf->NUM_POINTS) == i) {
        std::cout << "Wrong ids with :i:" << i << ";j:" << j << std::endl;
        exit(1);
    }
    if (i == j) {
        std::cout << "Wrong ids with :i:" << i << ";j:" << j << std::endl;
        exit(1);
    }
    const int length = j > i ? abs(j - i) : conf->NUM_POINTS - i + j;
    for (int l = 1; l <= length / 2; l++) {
#if defined(DEBUG)
        const int v1 = this->tour[cycle_id((i + l), conf->NUM_POINTS)], v2 = this->tour[cycle_id(j - l + 1, conf->NUM_POINTS)];
#endif
        buffer = this->tour[cycle_id((i + l), conf->NUM_POINTS)];
        this->tour[cycle_id((i + l), conf->NUM_POINTS)] = this->tour[cycle_id(j - l + 1, conf->NUM_POINTS)];
        this->tour[cycle_id(j - l + 1, conf->NUM_POINTS)] = buffer;
#if defined(DEBUG)
        if (v1 < 0 || v1 >= this->length || v2 < 0 || v2 >= this->length)
            exit(1);
#endif
    }
#if defined(DEBUG)
    if (this->check_tour() == 0)
        exit(1);
#endif
}
int Tour::check_tour() {
    for (int i = 0; i < this->length; i++) {
        if (this->tour[i] < 0 || this->tour[i] >= this->length) {
            return 0;
        }
    }
    return 1;
}
void Tour::describe() {
    using namespace std;
    cout << "\t[";
    for (int i = 0; i < this->length;i++) {
        cout << std::setw(2) << std::setfill('0') << this->tour[i] << ",";
    }
    cout << "]" << endl;
}
void Tour::describe(int start_id,int stop_id) {
    using namespace std;
    cout << "\t[";
    for (int i = 0; i < this->length; i++) {
        if ((i == 0 && stop_id < start_id) || i == start_id) {
            cout << "\x1B[31m";
        }
        cout << std::setw(2) << std::setfill('0') << this->tour[i] << ",";
        if (i == stop_id) {
            cout << "\x1B[37m";
        }
    }
    cout << "\x1B[37m";
    cout << "]" << endl;
}


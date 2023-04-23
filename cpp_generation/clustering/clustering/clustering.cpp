#include "clustering.h"

Clustering::Clustering(double* points, int* assignements, Config* config) : p_c(points), c_a(assignements) {
    this->n_p_p_c = new int[config->NUM_POINTS];
    for (int i = 0; i < config->NUM_POINTS; i++) {
        this->n_p_p_c[i] = 0;
    }
    this->c_c = new double[config->NUM_CLUST * config->NUM_DIM];
    for (int i = 0; i < config->NUM_CLUST * config->NUM_DIM; i++) {
        this->c_c[i] = 0.;
    }
}

Clustering::~Clustering() {
    delete[] this->p_c;
    delete[] this->c_a;
    delete[] this->n_p_p_c;
    delete[] this->c_c;
}

Clustering* Clustering::deepcopy(Config*config) {
    Clustering* new_clustering = new Clustering();
    // Initialize points coordinates
    new_clustering->p_c = new double[config->NUM_POINTS * config->NUM_DIM];
    for (int i = 0; i < config->NUM_POINTS * config->NUM_DIM; i++)
        new_clustering->p_c[i] = this->p_c[i];
    // Initialize number of points per cluster
    new_clustering->n_p_p_c = new int[config->NUM_CLUST];
    for (int i = 0; i < config->NUM_CLUST; i++)
        new_clustering->n_p_p_c[i] = this->n_p_p_c[i];
    // Initialize cluster intial assignements
    new_clustering->c_a = new int[config->NUM_POINTS];
    for (int i = 0; i < config->NUM_POINTS; i++)
        new_clustering->c_a[i] = this->c_a[i];
    // Initialize cluster centroids
    new_clustering->c_c = new double[config->NUM_CLUST * config->NUM_DIM];
    for (int i = 0; i < config->NUM_CLUST * config->NUM_DIM; i++)
        new_clustering->c_c[i] = this->c_c[i];
    return new_clustering;
}
void Clustering::print_with_coords(Config*conf, int pt_modif, int from, int to) {
    using namespace std;
    list<pair<int,double*>>* clust_pts = new list<pair<int, double*>>[conf->NUM_CLUST];
    // Pour chaque cluster je veux la liste des points
    for (int i = 0; i < conf->NUM_POINTS; i++) {
        clust_pts[this->c_a[i]].push_back(make_pair(i,&(this->p_c[i * conf->NUM_DIM])));
    }
    cout << "-------------Current clustering-------------" << endl;
    for (int i = 0; i < conf->NUM_CLUST; i++) {
        if (i == from || i == to) {
            cout << "\x1B[31m";
        }
        cout << "Cluster " << i << ":";
        if (i == from || i == to) {
            cout << "\x1B[37m";
        }
        for (pair<int,double*> point : clust_pts[i]) {
            if (point.first == pt_modif) {
                cout << "\x1B[31m";
            }
            cout << "(" << point.second[0] << "," << point.second[1] << "),";
            if (point.first == pt_modif) {
                cout << "\x1B[37m";
            }
        }
        cout << endl;
    }
    cout << "--------------------------------------------" << endl;

}
void Clustering::print_with_ids(Config* conf, int pt_modif, int from, int to) {
    using namespace std;
    list<int>* clust_pts = new list<int>[conf->NUM_CLUST];
    // Pour chaque cluster je veux la liste des points
    for (int i = 0; i < conf->NUM_POINTS; i++) {
        clust_pts[this->c_a[i]].push_back(i);
    }
    cout << "-------------Current clustering-------------" << endl;
    cout << pt_modif << " moved from " << from << " to " << to << endl;
    for (int i = 0; i < conf->NUM_CLUST; i++) {
        if (i == from || i == to) {
            cout << "\x1B[31m";
        }
        cout << "Cluster " << i << ":";
        if (i == from || i == to) {
            cout << "\x1B[37m";
        }
        for (int point : clust_pts[i]) {
            if (point == pt_modif) {
                cout << "\x1B[31m";
            }
            cout << point << ",";
            if (point == pt_modif) {
                cout << "\x1B[37m";
            }
        }
        cout << endl;
    }
    cout << "--------------------------------------------" << endl;

}
#include "moon.h"
double* moons(Config* config) {
    const float std_min = 0.01, std_max = 0.1;
    const int GRID_COORD_MIN = 0, GRID_COORD_MAX = 100;
    const double pi = 3.14159265358979323846;
    const double step = pi / double(config->NUM_POINTS / config->NUM_CLUST);
    std::mt19937 gen_noise(config->SEED_POINTS);
    std::uniform_real_distribution<double> dis_std(std_min, std_max);
    const double std_noise = dis_std(gen_noise);
    const double scale_factor = (double)(GRID_COORD_MAX - GRID_COORD_MIN) / (double)(2.);
    std::normal_distribution<double> dis_noise(0, std_noise);
    std::uniform_real_distribution<double> dis_theta(0, 2 * pi);
    double theta = dis_theta(gen_noise);
    double rotation[2][2] = {
        {cos(theta), -sin(theta)},
        {sin(theta),cos(theta)}
    };
    double* points = new double[config->NUM_POINTS * config->NUM_DIM];
    for (int i = 0; i < config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST; i++) {
        if (i % 2 == 0) {
            points[i] = (cos(step * (i / 2)) + 1) / 3.0;
        }
        else {
            points[i] = (sin(step * (i / 2)) + 0.5) / 1.5;
        }

        points[i] += dis_noise(gen_noise);
        if (i % 2 == 1) {
            points[i] -= 0.5;
            //manual matrixm multiplication
            double new_x = rotation[0][0] * points[i - 1] + rotation[0][1] * points[i];
            double new_y = rotation[1][0] * points[i - 1] + rotation[1][1] * points[i];
            points[i - 1] = new_x + 0.5;
            points[i] = new_y + 0.5;
            points[i - 1] = points[i - 1] * scale_factor + GRID_COORD_MIN;
            points[i] = points[i] * scale_factor + GRID_COORD_MIN;
        }
    }
    const int offset_pt = config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST;
    for (int i = 0; i < config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST; i++) {
        if (i % 2 == 0) {
            points[offset_pt + i] = (1 - cos(step * (i / 2)) + 1) / 3.0;
        }
        else {
            points[offset_pt + i] = (1 - sin(step * (i / 2)) - 0.5 + 0.5) / 1.5;
        }
        points[offset_pt + i] += dis_noise(gen_noise);
        if (i % 2 == 1) {
            points[offset_pt + i] -= 0.5;
            //manual matrix multiplication
            double new_x = rotation[0][0] * points[offset_pt + i - 1] + rotation[0][1] * points[offset_pt + i];
            double new_y = rotation[1][0] * points[offset_pt + i - 1] + rotation[1][1] * points[offset_pt + i];
            points[offset_pt + i - 1] = new_x + 0.5;
            points[offset_pt + i] = new_y + 0.5;
            points[offset_pt + i - 1] = points[offset_pt + i - 1] * scale_factor + GRID_COORD_MIN;
            points[offset_pt + i] = points[offset_pt + i] * scale_factor + GRID_COORD_MIN;
        }
    }
    return points;
}
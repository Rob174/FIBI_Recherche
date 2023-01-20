#include "kmeans.h"
// paper source k-means++: The Advantages of Careful Seeding David Arthurand Sergei Vassilvitskii
// comments citations from the paper

void kmeansPlusPlus(ClusteringContainer *clustering, ClusteringConfig *conf)
{
    const double GRID_MAX = 100;
    std::mt19937 gen_dist(M_cst_frc(SEED_ASSIGN));
    std::uniform_real_distribution<> dis_choice(0, GRID_MAX);
    // "D(x) denote the shortest distance from a data point to the closest center we have already chosen."
    //" Take one center c1, chosen uniformly at random from X"
    for (int i = 0; i < M_cst_frc(NUM_DIM); i++)
    {
        clustering->c_c[i] = dis_choice(gen_dist);
    }
    int num_centroids = 1;
    double *dist_matrix = new double[M_cst_frc(NUM_CLUST) * M_cst_frc(NUM_POINTS)];
    for (int i = 0; i < M_cst_frc(NUM_CLUST); i++)
    {
        for (int j = 0; j < M_cst_frc(NUM_POINTS); j++)
        {
            dist_matrix[i * M_cst_frc(NUM_POINTS) + j] = std::numeric_limits<double>::max();
        }
    }

    while (num_centroids < M_cst_frc(NUM_CLUST))
    {
        // Update distance matrix
        for (int i = num_centroids - 1; i < num_centroids; i++)
        {
            for (int j = 0; j < M_cst_frc(NUM_POINTS); j++)
            {
                dist_matrix[i * M_cst_frc(NUM_POINTS) + j] = dist(
                    &clustering->c_c[i * M_cst_frc(NUM_DIM)],
                    &clustering->p_c[j * M_cst_frc(NUM_DIM)],
                    M_cst_frc(NUM_DIM)
                    );
            }
        }
        // Assign point to closest centroid
        // At the same time, 1. Create weights for random discrete distribution according to formula:
        std::vector<double> weights;
        for (int i = 0; i < M_cst_frc(NUM_POINTS); i++)
        {
            // find closest centroid of point
            double closest_dist = std::numeric_limits<double>::max();
            int closest_clust = -1;
            for (int j = 0; j < M_cst_frc(NUM_CLUST); j++)
            {
                if (dist_matrix[j * M_cst_frc(NUM_POINTS) + i] < closest_dist)
                {
                    closest_dist = dist_matrix[j * M_cst_frc(NUM_POINTS) + i];
                    closest_clust = j;
                }
            }
            clustering->c_a[i] = closest_clust;
            weights.push_back(closest_dist);
        }
        // Find new centroid
        // 2. Find new centroid using random discrete distribution
        std::discrete_distribution<int> distribution(weights.begin(), weights.end());
        const int chosen_point = distribution(gen_dist);
        const double *point = &clustering->p_c[chosen_point * M_cst_frc(NUM_DIM)];
        const int start = num_centroids * M_cst_frc(NUM_DIM);
        for (int i = 0; i < M_cst_frc(NUM_DIM); i++)
        {
            clustering->c_c[start + i] = point[i];
        }
        num_centroids++;
    }

    compKmeansCentroids(clustering, conf);

    // Manage case where there are empty clusters
    // Affect the farthest points to the empty clusters

    // 1. Find then umber of required points num_pts_to_fill
    std::vector<int> empty_clusters;

    for (int i = 0; i < M_cst_frc(NUM_CLUST); i++)
    {
        if (clustering->n_p_p_c[i] == 0)
        {
            empty_clusters.push_back(i);
        }
    }
    const int num_pts_to_fill = empty_clusters.size();
    // 2. Find the num_pts_to_fill farthest away points
    // 2.1 Compute dist to centroids
    std::vector<std::pair<double, int>> d_to_clust;
    for (int i = 0; i < M_cst_frc(NUM_POINTS); i++)
    {
        const double *point = &clustering->p_c[i * M_cst_frc(NUM_DIM)];
        const int cluster = clustering->c_a[i];
        const double *clust_coords = &clustering->c_c[cluster * M_cst_frc(NUM_DIM)];
        d_to_clust.push_back(std::make_pair(dist(point, clust_coords, M_cst_frc(NUM_DIM)), i));
    }
    std::partial_sort(d_to_clust.rbegin(), d_to_clust.rbegin() + num_pts_to_fill, d_to_clust.rend(), std::greater{});
    for (int i = 0; i < num_pts_to_fill; i++)
    {
        auto &[d, idx_pt] = d_to_clust[i];
        const int origClusterPt = clustering->c_a[idx_pt];
        const int clustToFill = empty_clusters[i];
        clustering->c_a[idx_pt] = clustToFill;
        clustering->n_p_p_c[origClusterPt]--;
        clustering->n_p_p_c[clustToFill]++;
    }
    // Recompute centers of gravity
    for (int i = 0; i < M_cst_frc(NUM_CLUST) * M_cst_frc(NUM_DIM); i++)
    {
        clustering->c_c[i] = 0;
    }
    for (int i = 0; i < M_cst_frc(NUM_POINTS) * M_cst_frc(NUM_DIM); i++)
    {
        const double point_coord = clustering->p_c[i];
        const int cluster = clustering->c_a[i / M_cst_frc(NUM_DIM)];
        clustering->c_c[cluster * M_cst_frc(NUM_DIM) + i % M_cst_frc(NUM_DIM)] += point_coord;
    }
    for (int i = 0; i < M_cst_frc(NUM_CLUST) * M_cst_frc(NUM_DIM); i++)
    {
        clustering->c_c[i] /= clustering->n_p_p_c[i / M_cst_frc(NUM_DIM)];
    }
}
bool affectToNearestCluster(ClusteringContainer *clustering, ClusteringConfig *conf)
{
    for (int i = 0; i < M_cst_frc(NUM_CLUST); i++)
    {
        clustering->n_p_p_c[i] = 0;
    }
    // Update affectations and count of points per cluster
    for (int i = 0; i < M_cst_frc(NUM_POINTS); i++)
    {
        const double *c_points = &clustering->p_c[i * M_cst_frc(NUM_DIM)];
        double minDist = std::numeric_limits<double>::max();
        int chosenClust = -1;
        for (int c = 0; c < M_cst_frc(NUM_CLUST); c++)
        {
            const double *c_coords = &clustering->c_c[c * M_cst_frc(NUM_DIM)];
            const double d = dist(c_coords, c_points, M_cst_frc(NUM_DIM));
            if (d < minDist)
            {
                minDist = d;
                chosenClust = c;
            }
        }
        clustering->c_a[i] = chosenClust;
        clustering->n_p_p_c[chosenClust]++;
    }
    double *cpyCentroids = new double[M_cst_frc(NUM_CLUST) * M_cst_frc(NUM_DIM)];
    for (int i = 0; i < M_cst_frc(NUM_CLUST) * M_cst_frc(NUM_DIM); i++)
    {
        cpyCentroids[i] = clustering->c_c[i];
        clustering->c_c[i] = 0;
    }
    // Compute center of gravities (clusters centers)
    for (int i = 0; i < M_cst_frc(NUM_POINTS) * M_cst_frc(NUM_DIM); i++)
    {
        const double point_coord = clustering->p_c[i];
        const int cluster = clustering->c_a[i / M_cst_frc(NUM_DIM)];
        clustering->c_c[cluster * M_cst_frc(NUM_DIM) + i % M_cst_frc(NUM_DIM)] += point_coord;
    }
    bool oneCentHasChanged = false;
    for (int i = 0; i < M_cst_frc(NUM_CLUST) * M_cst_frc(NUM_DIM); i++)
    {
        clustering->c_c[i] /= clustering->n_p_p_c[i / M_cst_frc(NUM_DIM)];
        if (abs(clustering->c_c[i] - cpyCentroids[i]) > EPSILON)
        {
            oneCentHasChanged = true;
        }
    }
    delete[] cpyCentroids;
    return oneCentHasChanged;
}

void compKmeansCentroids(ClusteringContainer *clustering, ClusteringConfig *conf)
{
    bool changed = true;
    while (changed)
    {
        changed = affectToNearestCluster(clustering, conf);
    }
}
#include "metrics.h"

double get_overlap(Clustering* clust,Config*conf) {
	double overlap = 0;
	int count = 0;
	for (int i = 0; i < conf->NUM_POINTS; i++) {
		const double* p1 = &clust->p_c[i * conf->NUM_DIM];
		// distance from every point to its centroid(d1) 
		double d1 = dist(p1,&clust->c_c[clust->c_a[i]*conf->NUM_DIM], conf);
		// and to its nearest point in another cluster(d2)
		for (int j = 0; j < conf->NUM_POINTS; j++) {
			if (clust->c_a[i] == clust->c_a[j]) {
				continue;
			}
			const double* p2 = &clust->p_c[j * conf->NUM_DIM];
			const double d_nearest = dist(p1, p2, conf);
			// If this nearest point is closer than its own centroid(d1 > d2), the point is evidence of overlap
			if (d_nearest < d1) {
				overlap++;
			}
		}
		count++;
	}
	return overlap / (double)(count);
}

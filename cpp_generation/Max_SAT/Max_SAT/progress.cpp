#include "progress.h"
void Progress::print(const int status) {
	const double status_perc = (double)(status - start) / (num_elems);
	double time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time).count();
	int time_to_go_seconds = (int)(time_elapsed *(1./ status_perc-1.));

	int days, hours, min, seconds;
	days = time_to_go_seconds / (3600 * 24);
	hours = (time_to_go_seconds - days * 24 * 3600) / 3600;
	min = (time_to_go_seconds - days * 24 * 3600 - hours * 3600) / 60;
	seconds = (time_to_go_seconds - days * 24 * 3600 - hours * 3600 - min * 60);

	std::cout << " status " << std::fixed << std::setprecision(2) << status_perc*100 << "% ; ";
	std::cout << "time to go " << int(days) << "days" << int(hours) << "h" << int(min) << "min" << int(seconds) << "s ";
}
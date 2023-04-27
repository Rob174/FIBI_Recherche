#ifndef PROGRESS_H
#define PROGRESS_H
#include <ctime>
#include <chrono>
#include <iomanip>
#include <iostream>

class Progress {
private:
	std::chrono::steady_clock::time_point start_time;
	const int num_elems;
	const int start;
	const int stop;
public:
	Progress(const int start, const int stop) : num_elems(stop - start + 1),start(start),stop(stop), start_time(std::chrono::steady_clock::now()) {};
	void print(const int status);
};
#endif // !PROGRESS_H

#pragma once 
#include <chrono>
#include <time.h>
#include <iostream>

using namespace std;

class Progress {
private:
	const long num_total_elements;
	chrono::time_point<chrono::system_clock> start_time;
	chrono::time_point<chrono::system_clock> last_time_log;
	string color;
	string color_white = "\033[0m";
	string name;
public:
	Progress(const long num_total_elements, string color = "\033[31m",string name="") : num_total_elements(num_total_elements), start_time(chrono::system_clock::now()), color(color),name(name) {
		last_time_log = start_time;
		cout << "Num seeds to do : " << num_total_elements << endl;
	};
	void print_time(const long int seconds) {
		long seconds_cmpt = seconds;
		cout << seconds_cmpt / (3600 * 24) << " days,";
		seconds_cmpt = seconds_cmpt % (3600 * 24);
		cout << seconds_cmpt / 3600 << "H";
		seconds_cmpt = seconds_cmpt % 3600;
		cout << seconds_cmpt / 60 << "M";
		seconds_cmpt = seconds_cmpt % 60;
		cout << seconds_cmpt << "S";
	}
	void print_time(chrono::time_point<chrono::system_clock> time) {

		time_t now_time_t = chrono::system_clock::to_time_t(time);
		tm now_tm = *localtime(&now_time_t);
		cout << put_time(&now_tm, "%T");
	}
	void skip() {
		start_time = chrono::system_clock::now();
	}
	void print(const long iteration) {
		// Calculate progress percentages and time elapsed/remaining
		const double percentage_done = static_cast<double>(iteration) / num_total_elements * 100;
		auto now = chrono::system_clock::now();
		long int time_elapsed = chrono::duration_cast<chrono::seconds>(now - start_time).count();
		if (chrono::duration_cast<chrono::seconds>(now - last_time_log).count() < 5*60) {
			return;
		}
		// Print progress status
		cout << color;
		cout << name << ": ";
		cout << "Iteration " << iteration << "/" << num_total_elements << " (" << percentage_done << "%) Time ";
		print_time(now);
		cout << " ; "
			<< "Iterations done " << iteration << " (" << percentage_done << "%) Time elapsed ";
		print_time(time_elapsed);
		cout << " ; "
			<< "Iterations todo " << num_total_elements - iteration << " (" << 100 - percentage_done
			<< "%) Time estimated ";
		if (iteration > 0) {
			auto time_remaining_obj = chrono::duration_cast<chrono::seconds>(
				chrono::duration<long int>(num_total_elements - iteration) * time_elapsed / iteration);
			long int time_remaining = time_remaining_obj.count();
			print_time(time_remaining);
		}
		cout << " ; Started at ";
		print_time(start_time);
		cout << color_white << endl;
		last_time_log = chrono::system_clock::now();
	}
};

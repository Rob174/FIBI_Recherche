#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <algorithm>
// If linux include pthread
#ifdef __linux__ 
	//linux code goes here
#include <pthread.h>
#endif

using namespace std;

class ThreadPool {
private:
	vector<thread> threads_;
	queue<function<vector<pair<string, double>>(int)>> tasks_; // modified to take tasks returning bool
	mutex mutex_;
	condition_variable condition_;
	bool stop_ = false;
	int thread_count_ = 0;
	MergeMetadata& merger;
public:
	void add_metadata(vector<pair<string, double>> metadata) {
		unique_lock<mutex> lock(mutex_);
		this->merger.add_metadata(metadata);
	}
	ThreadPool(MergeMetadata& merger, int num_threads = -1): merger(merger) {
		if (num_threads == -1) {
			cout << thread::hardware_concurrency() << " threads";
			num_threads = thread::hardware_concurrency() - 2;
			if (num_threads <= 0) num_threads = 1;
		}
		cout << "Using " << num_threads << " threads" << endl;
		for (int i = 0; i < num_threads; ++i) {
			threads_.emplace_back([this, i] {
				int thread_id = i;
				while (true) {
					function<vector<pair<string, double>>(int)> task; // modified to take tasks returning bool
					{
						unique_lock<mutex> lock(mutex_);
						condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
						if (stop_ && tasks_.empty()) {
							return;
						}
						task = move(tasks_.front());
						tasks_.pop();
					}
					vector<pair<string, double>> result = task(thread_id); // modified to store the result of the task
					{
						unique_lock<mutex> lock(mutex_);
						// Check if merging is possible
						if (this->merger.ready_to_merge()) {
							this->merger.merge();
						}
						condition_.notify_all();
					}
				}
				});
		}
	}

	void join() {
		condition_.notify_all();
		for (auto& thread : threads_) {
			thread.join();
		}
		this->merger.merge();
	}
	~ThreadPool() {
		{
			unique_lock<mutex> lock(mutex_);
			stop_ = true;
		}
		condition_.notify_all();
		for (auto& thread : threads_) {
			thread.join();
		}
		this->merger.merge();
	}

	// Can submit functions taking two arguments: map<string,int> and int (thread_id) and returning a bool
	bool submit(std::function<vector<pair<string, double>>(std::map<std::string, int>, std::string, int)> task, std::map<std::string, int> args, std::string root_data) {
		auto bound_task = [this,task, args, root_data](int thread_id) {
			vector<pair<string, double>> res = task(args, root_data, thread_id); // modified to return the result of the task

			// Add seed done
			{
				std::unique_lock<std::mutex> lock(this->mutex_);
				this->merger.add_metadata(res);
			}
			return res;
		};
		bool stop = false;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (stop_) {
				return stop_;
			}
			tasks_.emplace(bound_task);
			while (tasks_.size() > 50) {
				condition_.wait(lock, [this] { return tasks_.size() <= 50; });
			}
		}
		condition_.notify_one();
		return false;
	}
};
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

using namespace std;

class ThreadPool {
private:
	vector<thread> threads_;
	queue<function<void(int)>> tasks_;
	mutex mutex_;
	condition_variable condition_;
	bool stop_ = false;
	int thread_count_ = 0;

public:
	ThreadPool(int num_threads = thread::hardware_concurrency()) {
		for (int i = 0; i < num_threads; ++i) {
			threads_.emplace_back([this, i] {
				int thread_id = i;
				while (true) {
					function<void(int)> task;
					{
						unique_lock<mutex> lock(mutex_);
						condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
						if (stop_ && tasks_.empty()) {
							return;
						}
						task = move(tasks_.front());
						tasks_.pop();
					}
					task(thread_id);
				}
				});
		}
	}
	void join() {
		condition_.notify_all();
		for (auto& thread : threads_) {
			thread.join();
		}
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
	}

	// Can submit functions taking two arguments: map<string,int> and int (thread_id)
	void submit(std::function<void(std::map<std::string, int>, int)> task, std::map<std::string, int> args) {
		auto bound_task = [task, args](int thread_id) {
			task(args, thread_id);
		};
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (stop_) {
				throw std::runtime_error("submit on stopped ThreadPool");
			}
			tasks_.emplace(bound_task);
			while (tasks_.size() > 50) {
				condition_.wait(lock);
			}
		}
		condition_.notify_one();
	}
};
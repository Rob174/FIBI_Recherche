#pragma once
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
// Taken from https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

class ThreadPool
{

private:
    std::queue<std::function<void(int)>> m_function_queue;
    std::mutex m_lock;
    std::condition_variable m_data_condition;
    std::atomic<bool> m_accept_functions;

public:

    ThreadPool();
    ~ThreadPool();
    void push(std::function<void(int)> func);
    void done();
    void infinite_loop_func(int i);
};
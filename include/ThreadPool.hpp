//
// Created by An Inconspicuous Semicolon on 17/12/2024.
//

#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool 
{
public:
    ThreadPool(unsigned int num_workers);
    ~ThreadPool();

    void enqueue_task(const std::function<void(void)>& function);

private:
    static void worker(ThreadPool* pool, unsigned int worker_id) noexcept;

private:
    std::vector<std::thread> m_workers;

    std::mutex m_tasks_mutex;
    std::condition_variable m_tasks_cv;
    std::queue<std::function<void(void)>> m_tasks;

    std::atomic<bool> m_quit;
};

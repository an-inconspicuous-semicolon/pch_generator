//
// Created by An Inconspicuous Semicolon on 17/12/2024.
//

#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(unsigned int num_workers)
    : m_quit(false)
{
    for (unsigned int i = 0; i < num_workers; ++i)
    {
        m_workers.push_back(std::thread(worker, this, i));
    }
}

ThreadPool::~ThreadPool()
{
    m_quit = true;
    for (auto& worker : m_workers)
        worker.join();
}

void ThreadPool::enqueue_task(const std::function<void(void)>& function)
{
    std::unique_lock lock(m_tasks_mutex);
    m_tasks.push(function);
    m_tasks_cv.notify_one();
}

void ThreadPool::worker(ThreadPool* pool, unsigned int worker_id) noexcept
{
    while (true)
    {
        std::unique_lock lock(pool->m_tasks_mutex);

        // Wait for tasks or quit signal
        pool->m_tasks_cv.wait(lock, [pool]()
        {
            return pool->m_quit || !pool->m_tasks.empty();
        });

        if (pool->m_quit && pool->m_tasks.empty())
            return;

        auto task = std::move(pool->m_tasks.front());
        pool->m_tasks.pop();

        lock.unlock();
        task();
    }
}

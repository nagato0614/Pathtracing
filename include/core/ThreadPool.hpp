//
// https://vorbrodt.blog/2019/02/12/simple-thread-pool/
//

#ifndef PATHTRACING_THREADPOOL_HPP
#define PATHTRACING_THREADPOOL_HPP

#include <cassert>
#include <functional>
#include <future>
#include <thread>
#include <vector>
#include "core/BlockingQueue.hpp"

class ThreadPool
{
  public:
    using Proc = std::function<void()>;

    ThreadPool(unsigned int queueDepth = std::thread::hardware_concurrency(),
               size_t threads = std::thread::hardware_concurrency()) : m_workQueue(queueDepth)
    {
      assert(queueDepth != 0);
      assert(threads != 0);
      for (size_t i = 0; i < threads; ++i)
        m_threads.emplace_back(std::thread(
          [this]()
          {
            while (true)
            {
              auto workItem = m_workQueue.pop();
              if (workItem == nullptr)
              {
                m_workQueue.push(nullptr);
                break;
              }
              workItem();
            }
          }));
    }

    ~ThreadPool() noexcept
    {
      m_workQueue.push(nullptr);
      for (auto &thread : m_threads)
        thread.join();
    }

    template<typename F, typename... Args>
    void enqueue_work(F &&f, Args &&...args)
    {
      m_workQueue.push([=]() { f(args...); });
    }

    template<typename F, typename... Args>
    auto enqueue_task(F &&f, Args &&...args)
      -> std::future<typename std::invoke_result<F, Args...>::type>
    {
      using return_type = typename std::invoke_result<F, Args...>::type;
      auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
      std::future<return_type> res = task->get_future();
      m_workQueue.push([task]() { (*task)(); });
      return res;
    }

  private:
    std::vector<std::thread> m_threads;
    blocking_queue<Proc> m_workQueue;
};
#endif // PATHTRACING_THREADPOOL_HPP

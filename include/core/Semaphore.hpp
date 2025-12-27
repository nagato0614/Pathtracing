//
// Created by toru on 2025/12/27.
//

#ifndef PATHTRACING_SEMAPHORE_HPP
#define PATHTRACING_SEMAPHORE_HPP
#include <atomic>
#include <cassert>
#include <condition_variable>

class Semaphore
{
  public:
    explicit Semaphore(const int count) noexcept : m_count(count) { assert(count > -1); }

    void post() noexcept
    {
      {
        std::unique_lock lock(m_mutex);
        ++m_count;
      }
      m_cv.notify_one();
    }

    void wait() noexcept
    {
      std::unique_lock lock(m_mutex);
      m_cv.wait(lock, [&]() { return m_count != 0; });
      --m_count;
    }

  private:
    int m_count;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

class fast_semaphore
{
  public:
    fast_semaphore(int count) noexcept : m_count(count), m_semaphore(0) {}

    void post()
    {
      std::atomic_thread_fence(std::memory_order_release);
      int count = m_count.fetch_add(1, std::memory_order_relaxed);
      if (count < 0)
        m_semaphore.post();
    }

    void wait()
    {
      int count = m_count.fetch_sub(1, std::memory_order_relaxed);
      if (count < 1)
        m_semaphore.wait();
      std::atomic_thread_fence(std::memory_order_acquire);
    }

  private:
    std::atomic<int> m_count;
    Semaphore m_semaphore;
};

#endif // PATHTRACING_SEMAPHORE_HPP

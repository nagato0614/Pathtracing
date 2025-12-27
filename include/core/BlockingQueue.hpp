//
// https://vorbrodt.blog/2019/02/09/template-concepts-sort-of/
//

#ifndef PATHTRACING_BLOCKINGQUEUE_HPP
#define PATHTRACING_BLOCKINGQUEUE_HPP

#include "core/Semaphore.hpp"

template<class T>
class blocking_queue
{
  public:
    blocking_queue(unsigned int size) :
        m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
        m_data((T *) operator new(size * sizeof(T))), m_openSlots(size), m_fullSlots(0)
    {
    }

    blocking_queue(const blocking_queue &) = delete;
    blocking_queue(blocking_queue &&) = delete;
    blocking_queue &operator=(const blocking_queue &) = delete;
    blocking_queue &operator=(blocking_queue &&) = delete;

    ~blocking_queue() noexcept
    {
      while (m_count--)
      {
        m_data[m_popIndex].~T();
        m_popIndex = ++m_popIndex % m_size;
      }
      operator delete(m_data);
    }

    template<typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value and
                                       std::is_nothrow_copy_constructible<U>::value, int>::type = 0>
    void push(const T &item) noexcept
    {
      m_openSlots.wait();
      {
        std::lock_guard<std::mutex> lock(m_cs);
        new (m_data + m_pushIndex) T(item);
        m_pushIndex = ++m_pushIndex % m_size;
        ++m_count;
      }
      m_fullSlots.post();
    }

    template<typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value and
                                       not std::is_nothrow_copy_constructible<U>::value, int>::type = 0>
    void push(const T &item)
    {
      m_openSlots.wait();
      {
        std::lock_guard<std::mutex> lock(m_cs);
        try
        {
          new (m_data + m_pushIndex) T(item);
        } catch (...)
        {
          m_openSlots.post();
          throw;
        }
        m_pushIndex = ++m_pushIndex % m_size;
        ++m_count;
      }
      m_fullSlots.post();
    }

    template<typename U = T, typename std::enable_if<std::is_move_constructible<U>::value and
                                       std::is_nothrow_move_constructible<U>::value, int>::type = 0>
    void push(T &&item) noexcept
    {
      m_openSlots.wait();
      {
        std::lock_guard<std::mutex> lock(m_cs);
        new (m_data + m_pushIndex) T(std::move(item));
        m_pushIndex = ++m_pushIndex % m_size;
        ++m_count;
      }
      m_fullSlots.post();
    }

    template<typename U = T, typename std::enable_if<std::is_move_constructible<U>::value and
                                       not std::is_nothrow_move_constructible<U>::value, int>::type = 0>
    void push(T &&item)
    {
      m_openSlots.wait();
      {
        std::lock_guard<std::mutex> lock(m_cs);
        try
        {
          new (m_data + m_pushIndex) T(std::move(item));
        } catch (...)
        {
          m_openSlots.post();
          throw;
        }
        m_pushIndex = ++m_pushIndex % m_size;
        ++m_count;
      }
      m_fullSlots.post();
    }

    template<typename U = T, typename std::enable_if<not std::is_move_assignable<U>::value and
                                       std::is_nothrow_copy_assignable<U>::value, int>::type = 0>
    void pop(T &item) noexcept
    {
      m_fullSlots.wait();
      {
        std::lock_guard<std::mutex> lock(m_cs);
        item = m_data[m_popIndex];
        m_data[m_popIndex].~T();
        m_popIndex = ++m_popIndex % m_size;
        --m_count;
      }
      m_openSlots.post();
    }

    template<typename U = T, typename std::enable_if<not std::is_move_assignable<U>::value and
                                       not std::is_nothrow_copy_assignable<U>::value, int>::type = 0>
    void pop(T &item)
    {
      m_fullSlots.wait();
      {
        std::lock_guard<std::mutex> lock(m_cs);
        try
        {
          item = m_data[m_popIndex];
        } catch (...)
        {
          m_fullSlots.post();
          throw;
        }
        m_data[m_popIndex].~T();
        m_popIndex = ++m_popIndex % m_size;
        --m_count;
      }
      m_openSlots.post();
    }

    template<typename U = T, typename std::enable_if<std::is_move_assignable<U>::value and
                                       std::is_nothrow_move_assignable<U>::value, int>::type = 0>
    void pop(T &item) noexcept
    {
      m_fullSlots.wait();
      {
        std::lock_guard<std::mutex> lock(m_cs);
        item = std::move(m_data[m_popIndex]);
        m_data[m_popIndex].~T();
        m_popIndex = ++m_popIndex % m_size;
        --m_count;
      }
      m_openSlots.post();
    }

    template<typename U = T, typename std::enable_if<std::is_move_assignable<U>::value and
                                       not std::is_nothrow_move_assignable<U>::value, int>::type = 0>
    void pop(T &item)
    {
      m_fullSlots.wait();
      {
        std::lock_guard<std::mutex> lock(m_cs);
        try
        {
          item = std::move(m_data[m_popIndex]);
        } catch (...)
        {
          m_fullSlots.post();
          throw;
        }
        m_data[m_popIndex].~T();
        m_popIndex = ++m_popIndex % m_size;
        --m_count;
      }
      m_openSlots.post();
    }

    T pop() noexcept(std::is_nothrow_move_assignable<T>::value)
    {
      T item;
      pop(item);
      return item;
    }

    bool empty() const noexcept
    {
      std::lock_guard lock(m_cs);
      return m_count == 0;
    }

    bool size() const noexcept
    {
      std::lock_guard lock(m_cs);
      return m_count;
    }

    unsigned int max_size() const noexcept { return m_size; }

  private:
    const unsigned int m_size;
    unsigned int m_pushIndex;
    unsigned int m_popIndex;
    unsigned int m_count;
    T *m_data;

    fast_semaphore m_openSlots;
    fast_semaphore m_fullSlots;
    std::mutex m_cs;
};

#endif // PATHTRACING_BLOCKINGQUEUE_HPP

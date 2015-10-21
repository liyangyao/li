/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/23

****************************************************************************/

#ifndef LI_BLOCKINGQUEUE_H
#define LI_BLOCKINGQUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <QDebug>

namespace li {

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue() = default;
    void put(const T& x)
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_queue.emplace_back(x);
        m_cond.notify_one();
    }
    void put(T&& x)
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_queue.emplace_back(std::forward<T>(x));
        m_cond.notify_one();
    }

    T take()
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_cond.wait(locker, [this]{return !m_queue.empty();});
        T x = std::move(m_queue.front());
        m_queue.pop_front();
        return x;
    }

    BlockingQueue(BlockingQueue &) = delete;
    BlockingQueue& operator=(BlockingQueue &) = delete;

private:
    std::deque<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};
}

#endif // LI_BLOCKINGQUEUE_H

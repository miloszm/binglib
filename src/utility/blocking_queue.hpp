#ifndef BLOCKING_QUEUE_HPP
#define BLOCKING_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <deque>

template <typename T>
class blocking_queue {
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::deque<T> queue_;
public:
    void push(T const& value) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push_front(value);
        }
        condition_.notify_one();
    }
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [=]{ return !queue_.empty(); });
        T value(std::move(queue_.back()));
        queue_.pop_back();
        return value;
    }
};

#endif
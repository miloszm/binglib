/**
 * Copyright (c) 2020-2021 binglib developers (see AUTHORS)
 *
 * This file is part of binglib.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
    void push(T value) {
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
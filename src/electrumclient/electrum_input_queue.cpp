#include "electrum_input_queue.hpp"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;
using json = nlohmann::json;
using namespace std;


void ElectrumInputQueue::push(const ElectrumMessage& message) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push_front(message);
    }
    condition_.notify_all();
}

ElectrumMessage ElectrumInputQueue::pop_reply(int id) {
    std::unique_lock<std::mutex> lock(mutex_);
    int factor = 16;
    for(int i = 0; i < 9; ++i) {
        if (condition_.wait_for(lock, factor*5ms, [=] {
            return contains_msg_with_id(id);
            }))
        {
            auto iter = find_msg_with_id(id);
            ElectrumMessage m(std::move(*iter));
            queue_.erase(iter);
            return m;
        } else {
            factor *= 2;
            continue;
        }
    }
    throw std::invalid_argument(string("electrum response timeout for correlation id=") + to_string(id));
}

void ElectrumInputQueue::pop_eat_reply(int id) {
    try {
        pop_reply(id);
    } catch (exception& e){
        // suppressing exceptions on purpose here
    }
}

bool ElectrumInputQueue::contains_msg_with_id(int id) {
    return find_msg_with_id(id) != queue_.end();
}

std::deque<ElectrumMessage>::const_iterator ElectrumInputQueue::find_msg_with_id(int id) {
    return std::find_if(queue_.begin(), queue_.end(), [=] (const ElectrumMessage& message) {
        return message.has_correlation_id && message.correlation_id == id;
    });
}

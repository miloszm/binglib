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
    for(int i = 1; i <= 10; ++i) {
        if (condition_.wait_for(lock, i*100ms, [=] {
            return contains_msg_with_id(id);
            }))
        {
            auto iter = find_msg_with_id(id);
            ElectrumMessage m(std::move(*iter));
            queue_.erase(iter);
            //cout << "gotten response for corr id " << id << " in try no " << i << "\n";
            return m;
        } else {
            continue;
        }
    }
    cout << "electrum response missing for correlation id=" << id << "\n";
    // TODO don't know at the moment what is best to do in such case
}

bool ElectrumInputQueue::contains_msg_with_id(int id) {
    return find_msg_with_id(id) != queue_.end();
}

std::deque<ElectrumMessage>::const_iterator ElectrumInputQueue::find_msg_with_id(int id) {
    return std::find_if(queue_.begin(), queue_.end(), [=] (const ElectrumMessage& message) {
        return message.has_correlation_id && message.correlation_id == id;
    });
}

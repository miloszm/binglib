#include "electrum_input_queue.hpp"

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
    condition_.wait(lock, [=] {
        return contains_msg_with_id(id);
    });
    auto iter = find_msg_with_id(id);
    ElectrumMessage m(std::move(*iter));
    queue_.erase(iter);
    return m;
}

ElectrumMessage ElectrumInputQueue::pop_subscription_msg(int id) {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [=] {
        return contains_subscription_msg();
    });
    auto iter = find_subscription_msg();
    ElectrumMessage m(std::move(*iter));
    queue_.erase(iter);
    return m;
}

bool ElectrumInputQueue::contains_msg_with_id(int id) {
    return find_msg_with_id(id) != queue_.end();
}

std::deque<ElectrumMessage>::const_iterator ElectrumInputQueue::find_msg_with_id(int id) {
    return std::find_if(queue_.begin(), queue_.end(), [=] (const ElectrumMessage& message) {
        return message.has_correlation_id && message.correlation_id == id;
    });
}

bool ElectrumInputQueue::contains_subscription_msg() {
    return find_subscription_msg() != queue_.end();
}

std::deque<ElectrumMessage>::const_iterator ElectrumInputQueue::find_subscription_msg() {
    return std::find_if(queue_.begin(), queue_.end(), [=] (const ElectrumMessage& message) {
        return !message.has_correlation_id;
    });
}

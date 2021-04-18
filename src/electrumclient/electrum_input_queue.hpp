#ifndef ELECTRUM_INPUT_QUEUE_HPP
#define ELECTRUM_INPUT_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <deque>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;


struct ElectrumMessage {
    json message;
    string method;
    bool has_correlation_id;
    int correlation_id;
};


class ElectrumInputQueue {
private:
    std::mutex                  mutex_;
    std::condition_variable     condition_;
    std::deque<ElectrumMessage> queue_;
public:
    void push(const ElectrumMessage& message);
    ElectrumMessage pop_reply(int id);
    vector<ElectrumMessage> copyAll(){vector<ElectrumMessage> v(queue_.begin(), queue_.end()); return v;}
private:
    bool contains_msg_with_id(int id);
    std::deque<ElectrumMessage>::const_iterator find_msg_with_id(int id);
};

#endif
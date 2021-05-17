#ifndef ELECTRUM_INPUT_QUEUE_HPP
#define ELECTRUM_INPUT_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <deque>
#include <nlohmann/json.hpp>
#include "binglib/electrum_model.hpp"
//#include "src/electrumclient/electrum_model.hpp"

using json = nlohmann::json;
using namespace std;



class RonghuaInputQueue {
private:
    std::mutex                  mutex_;
    std::condition_variable     condition_;
    std::deque<ElectrumMessage> queue_;
public:
    void push(const ElectrumMessage& message);
    ElectrumMessage pop_reply(int id);
    vector<ElectrumMessage> pop_reply_bulk(vector<int> ids);
    void pop_eat_reply(int id);
    vector<ElectrumMessage> copyAll(){vector<ElectrumMessage> v(queue_.begin(), queue_.end()); return v;}
private:
    bool contains_msg_with_id(int id);
    bool contains_msg_with_id_bulk(vector<int> ids);
    std::deque<ElectrumMessage>::const_iterator find_msg_with_id(int id);
};

#endif
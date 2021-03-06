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
#include "ronghua_input_queue.hpp"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using json = nlohmann::json;
using namespace std;


void RonghuaInputQueue::push(const ElectrumMessage& message) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push_front(message);
    }
    condition_.notify_all();
}

ElectrumMessage RonghuaInputQueue::pop_reply(int id) {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [=] {
        return contains_msg_with_id(id);
    });
    auto iter = find_msg_with_id(id);
    ElectrumMessage m(std::move(*iter));
    queue_.erase(iter);
    return m;
}

vector<ElectrumMessage> RonghuaInputQueue::pop_reply_bulk(vector<int> ids) {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait_for(lock, 1500ms, [=] {
        return contains_msg_with_id_bulk(ids);
    });
    vector<ElectrumMessage> messages;
    for (int id: ids) {
        auto iter = find_msg_with_id(id);
        if (iter == queue_.end()){
            break;
        }
        ElectrumMessage m(std::move(*iter));
        queue_.erase(iter);
        messages.push_back(m);
    }
    return messages;
}

void RonghuaInputQueue::pop_eat_reply(int id) {
    pop_reply(id);
}

bool RonghuaInputQueue::contains_msg_with_id(int id) {
    return find_msg_with_id(id) != queue_.end();
}

bool RonghuaInputQueue::contains_msg_with_id_bulk(vector<int> ids) {
    for (int id: ids) {
        if(find_msg_with_id(id) == queue_.end()) {
            return false;
        }
    }
    return true;
}

std::deque<ElectrumMessage>::const_iterator RonghuaInputQueue::find_msg_with_id(int id) {
    return std::find_if(queue_.begin(), queue_.end(), [=] (const ElectrumMessage& message) {
        if (id >= 0) {
            return message.has_correlation_id && message.correlation_id == id;
        } else {
            return !message.has_correlation_id;
        }
    });
}

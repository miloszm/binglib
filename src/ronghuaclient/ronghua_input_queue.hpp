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

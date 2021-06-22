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
#define BOOST_TEST_MODULE bing_test
#include <boost/test/included/unit_test.hpp>

#include <binglib/electrum_input_queue.hpp>
#include <binglib/json_socket_client.hpp>
#include <iostream>

using namespace std;



BOOST_AUTO_TEST_CASE(electrum_input_queue_test)
{
    ElectrumInputQueue queue;

    for (int i = 1; i < 4; i++){
        ElectrumMessage message {json::parse("{}"), "", true, i};
        queue.push(message);
    }

    ostringstream oss_before;
    ostringstream oss_after;
    for (ElectrumMessage& m: queue.copyAll()){
        oss_before << m.correlation_id;
    }
    queue.pop_reply(2);
    for (ElectrumMessage& m: queue.copyAll()){
        oss_after << m.correlation_id;
    }
    BOOST_TEST(oss_before.str() == "321");
    BOOST_TEST(oss_after.str() == "31");
    queue.pop_reply(3);
    queue.pop_reply(1);
    BOOST_TEST(queue.copyAll().empty() == true);
}

BOOST_AUTO_TEST_CASE(electrum_message_from_json_test)
{
    json msg_json_1 = json::parse(R"({"jsonrpc":"2.0","method":"ping","id":17})");
    ElectrumMessage message1 = JsonSocketClient::from_json(msg_json_1);
    BOOST_TEST(message1.method == "ping");
    BOOST_TEST(message1.has_correlation_id == true);
    BOOST_TEST(message1.correlation_id == 17);

    json msg_json_2 = json::parse(R"({"jsonrpc":"2.0","id":18})");
    ElectrumMessage message2 = JsonSocketClient::from_json(msg_json_2);
    BOOST_TEST(message2.method == "");
    BOOST_TEST(message2.has_correlation_id == true);
    BOOST_TEST(message2.correlation_id == 18);

    json msg_json_3 = json::parse(R"({"jsonrpc":"2.0","method":"ping2"})");
    ElectrumMessage message3 = JsonSocketClient::from_json(msg_json_3);
    BOOST_TEST(message3.method == "ping2");
    BOOST_TEST(message3.has_correlation_id == false);

    json msg_json_4 = json::parse(R"({"jsonrpc":"2.0","method":"ping2","params": ["p1","p2"]})");
    ElectrumMessage message4 = JsonSocketClient::from_json(msg_json_4);
    BOOST_TEST(message4.method == "ping2");
    BOOST_TEST(message4.has_correlation_id == false);
    BOOST_TEST(message4.params.size() == 2);
    BOOST_TEST(message4.params[0] == "p1");
    BOOST_TEST(message4.params[1] == "p2");
}

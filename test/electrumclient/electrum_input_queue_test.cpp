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
}

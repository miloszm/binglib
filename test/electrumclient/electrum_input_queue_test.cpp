#define BOOST_TEST_MODULE bing_test
#include <boost/test/included/unit_test.hpp>

#include <binglib/electrum_input_queue.hpp>
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

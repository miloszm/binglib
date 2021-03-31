#define BOOST_TEST_MODULE bing_test
#include <boost/test/included/unit_test.hpp>

#include "utility/address_converter.hpp"
#include <iostream>

using namespace std;



BOOST_AUTO_TEST_CASE(address_conversion_test)
{
    string spkh_hex = AddressConverter::base58_to_spkh_hex("mpS14bFCZiHFRxfNNbnPT2FScJBrm96iLE");
    BOOST_TEST(spkh_hex == "af89af88915ddf9ee02b223800d66aec14e01bb523bd870c6c358fb935d9f004");
}

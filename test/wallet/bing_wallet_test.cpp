#define BOOST_TEST_MODULE bing_test
#include <boost/test/included/unit_test.hpp>

#include "src/common/bing_common.hpp"
#include "wallet/bing_wallet.hpp"
#include <iostream>

using namespace std;

BOOST_AUTO_TEST_CASE(address_derivation_test) {
  vector<string> addresses;
  map<string, ec_private> addresses_to_ec_private;
  BingWallet::derive_addresses(true,
                               "effort canal zoo clown shoulder genuine "
                               "penalty moral unit skate few quick",
                               5, addresses, addresses_to_ec_private);
  BOOST_TEST(addresses.at(4) == "mnasBq7AAYddaYZptduGNQkLdXQjb7PRKt");
}

#define BOOST_TEST_MODULE bing_test
#include <boost/test/included/unit_test.hpp>

#include <binglib/bing_common.hpp>
#include "wallet/bing_wallet.hpp"
#include <iostream>

using namespace std;

BOOST_AUTO_TEST_CASE(address_derivation_testnet_test) {
  vector<string> addresses;
  map<string, AddressDerivationResult> addresses_to_data;
  BingWallet::derive_electrum_addresses(true,
                               "effort canal zoo clown shoulder genuine "
                               "penalty moral unit skate few quick",
                               5, 5, addresses, addresses_to_data);
  string expected0 = "mnasBq7AAYddaYZptduGNQkLdXQjb7PRKt";
  string expected1 = "mywJUCJbdn36wHTMeCeUFzuLx3F2ybvpdD";
  BOOST_TEST(addresses.at(4) == expected0);
  BOOST_TEST(addresses_to_data[expected0].derivation_path == "m/0/4");
  BOOST_TEST(addresses.at(9) == expected1);
  BOOST_TEST(addresses_to_data[expected1].derivation_path == "m/1/4");
}

BOOST_AUTO_TEST_CASE(address_derivation_mainnet_test) {
  vector<string> addresses;
  map<string, AddressDerivationResult> addresses_to_data;
  BingWallet::derive_electrum_addresses(false,
                               "effort canal zoo clown shoulder genuine "
                               "penalty moral unit skate few quick",
                               5, 0, addresses, addresses_to_data);
  string expected = "184utn2BMXCNoS6DB4vtYVY1mXp2ZiNETW";
  BOOST_TEST(addresses.at(4) == expected);
  BOOST_TEST(addresses_to_data[expected].derivation_path == "m/0/4");
}

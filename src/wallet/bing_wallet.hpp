#ifndef BING_WALLET_HPP
#define BING_WALLET_HPP

#include <bitcoin/system.hpp>
using namespace bc::wallet;
#include <string>

using namespace std;

struct AddressDerivationResult {
  ec_private priv_key_ec;
  string derivation_path;
};

/**
 * derives `count` of addresses and a corresponding address to ec_private map
 * from a given seed_phrase
 * count0 is the number of addresses with path m/0/x ("receiving" in Electrum speak)
 * count1 is the number of addresses with path m/1/x ("change" in Electrum speak)
 */
class BingWallet {
public:
  static void
  derive_electrum_addresses(bool testnet, const string seed_phrase, int count0, int count1,
                   vector<string> &addresses,
                   map<string, AddressDerivationResult> &addresses_to_data);
};

#endif
#ifndef BING_WALLET_HPP
#define BING_WALLET_HPP

#include <bitcoin/bitcoin.hpp>
using namespace bc::wallet;
#include <string>

using namespace std;

/**
 * derives `count` of addresses and corresponding address to ec_private map
 * from a given seed_phrase
 */
class BingWallet {
public:
    static void derive_addresses(bool testnet, const string seed_phrase, int count, vector<string>& addresses, map<string, ec_private>& addresses_to_ec_private);
};


#endif
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
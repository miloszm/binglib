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
#include "src/common/bing_common.hpp"
#define WITH_ICU 1
#include <bitcoin/system/wallet/electrum.hpp>
#include "bing_wallet.hpp"

using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;

void BingWallet::derive_electrum_addresses(
    bool testnet, const string seed_phrase, int count0, int count1,
    vector<string> &addresses,
    map<string, AddressDerivationResult> &addresses_to_data) {

  addresses.clear();
  addresses_to_data.clear();

  const word_list mnemonic = split(seed_phrase, " ");
  if (!wallet::electrum::validate_mnemonic(mnemonic, language::en))
    throw std::invalid_argument("incorrect mnemonic");

  long_hash seed = wallet::electrum::decode_mnemonic(mnemonic);
  data_chunk seedAsChunk(seed.begin(), seed.end());

  uint64_t testnet_or_mainnet{0};
  if (testnet)
    testnet_or_mainnet = hd_private::testnet;
  else
    testnet_or_mainnet = hd_private::mainnet;

  const hd_private m(seedAsChunk, testnet_or_mainnet);
  const hd_public m_pub = m;

  uint8_t payment_address_version{0};
  if (testnet)
      payment_address_version = payment_address::testnet_p2kh;
  else
      payment_address_version = payment_address::mainnet_p2kh;

  vector<int> counters {count0, count1};
  for (int i = 0; i < 2; ++i) {
      hd_private mx = m.derive_private(i);
      for (int j = 0; j < counters.at(i); ++j) {
          hd_private hdPrivate = mx.derive_private(j);
          const payment_address address(
                  {hdPrivate.secret(), payment_address_version});
          addresses.push_back(address.encoded());
          addresses_to_data[address.encoded()] = AddressDerivationResult{
                  ec_private(hdPrivate.secret(), payment_address_version),
                  "m/" + to_string(i) + "/" + to_string(j)
          };
      }
  }
}

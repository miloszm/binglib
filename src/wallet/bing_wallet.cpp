#include "bing_wallet.hpp"
#include "src/common/bing_common.hpp"

using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;

void BingWallet::derive_addresses(
    bool testnet, const string seed_phrase, int count,
    vector<string> &addresses,
    map<string, ec_private> &addresses_to_ec_private) {

  const word_list mnemonic = split(seed_phrase, " ");
  if (!electrum::validate_mnemonic(mnemonic, language::en))
    throw std::invalid_argument("incorrect mnemonic");

  long_hash seed = electrum::decode_mnemonic(mnemonic);
  data_chunk seedAsChunk(seed.begin(), seed.end());

  uint64_t testnet_mainnet{hd_private::mainnet};
  if (testnet)
    testnet_mainnet = hd_private::testnet;
  const hd_private m(seedAsChunk, testnet_mainnet);
  const hd_public m_pub = m;

  auto m0_pub = m.derive_public(0);
  auto m1_pub = m.derive_public(1);

  hd_private m0 = m.derive_private(0);

  for (int i = 0; i < count; ++i) {
    hd_private hdPrivate = m0.derive_private(i);
    const payment_address address(
        {hdPrivate.secret(), payment_address::testnet_p2kh});
    addresses.push_back(address.encoded());
    addresses_to_ec_private[address.encoded()] =
        ec_private(hdPrivate.secret(), payment_address::testnet_p2kh);
  }
}

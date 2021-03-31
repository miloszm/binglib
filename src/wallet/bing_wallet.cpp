#include "src/common/bing_common.hpp"
#include "bing_wallet.hpp"

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

  uint64_t testnet_or_mainnet{0};
  if (testnet)
    testnet_or_mainnet = hd_private::testnet;
  else
    testnet_or_mainnet = hd_private::mainnet;

  const hd_private m(seedAsChunk, testnet_or_mainnet);
  const hd_public m_pub = m;

  auto m0_pub = m.derive_public(0);
  auto m1_pub = m.derive_public(1);

  hd_private m0 = m.derive_private(0);

  uint8_t payment_address_version{0};
  if (testnet)
      payment_address_version = payment_address::testnet_p2kh;
  else
      payment_address_version = payment_address::mainnet_p2kh;

  for (int i = 0; i < count; ++i) {
    hd_private hdPrivate = m0.derive_private(i);
    const payment_address address(
        {hdPrivate.secret(), payment_address_version});
    addresses.push_back(address.encoded());
    addresses_to_ec_private[address.encoded()] =
        ec_private(hdPrivate.secret(), payment_address_version);
  }
}

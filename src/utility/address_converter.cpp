#include "src/common/bing_common.hpp"
#include "address_converter.hpp"
#include <bitcoin/bitcoin.hpp>
#include <vector>
#include <iterator>

using namespace std;
using namespace bc;

string AddressConverter::base58_to_spkh_hex(string base58_address){
    data_chunk decoded;
    if (!decode_base58(decoded, base58_address))
        throw std::invalid_argument("base58_to_spk_hex");
    data_chunk stripped(decoded.begin()+1, decoded.begin()+21);
    data_chunk prefix{ 0x76, 0xa9, 0x14};
    const data_chunk postfix{ 0x88, 0xac};
    stripped.insert(stripped.end(), postfix.begin(), postfix.end());
    stripped.insert(stripped.begin(), prefix.begin(), prefix.end());
    data_slice sl(stripped);
    data_chunk sha {sha256_hash_chunk(sl)};
    data_chunk sha_rev(sha.rbegin(), sha.rend());
    ostringstream os;
    os << libbitcoin::config::base16(sha_rev);
    return os.str();
}

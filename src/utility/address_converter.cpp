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
#include "address_converter.hpp"
#include <bitcoin/system.hpp>
#include <iterator>

using namespace std;
using namespace bc;

/**
 * Note:
 * Base58 to spkh (Script Pub Key Hash) conversion
 * is needed as key to Electrum server and EPS/EPSMI address history
 * as used for example in:
 * blockchain.scripthash.get_history
 *
 * how this conversion works:
 *  say we have a base58 address: mpS14bFCZiHFRxfNNbnPT2FScJBrm96iLE
 *  we convert it to bytes via bx command:
 *  bx base58-decode mpS14bFCZiHFRxfNNbnPT2FScJBrm96iLE
 *  6f61c95cddadf465cac9b0751edad16624d01572c066ff8027
 *  first byte is ?, last 4 bytes are checksum in little endian
 *  actual address is:
 *  61c95cddadf465cac9b0751edad16624d01572c0
 *  the rest needs to have 76a914 prepended and 88ac appended:
 *  OP_DUP OP_HASH160 20 (20 for following data length) OP_EQUALVERIFY OP_CHECKSIG
 *  76a91461c95cddadf465cac9b0751edad16624d01572c088ac
 *  then we sha256 it via bx command:
 *  bx sha256 76a91461c95cddadf465cac9b0751edad16624d01572c088ac
 *  04f0d935b98f356c0c87bd23b51be014ec6ad60038222be09edf5d9188af89af
 *  then we need to reverse and convert to hex:
 *  af89af88915ddf9ee02b223800d66aec14e01bb523bd870c6c358fb935d9f004
 *
 *  Note: this works only for legacy addresses (non-segregated witness)
 */

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

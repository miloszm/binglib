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
#include "redeem_script.hpp"

using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;

/**
 * locks funds until "lock_until" number of seconds since Jan 1st 1970
 */

operation::list
RedeemScript::to_pay_key_hash_pattern_with_lock(const data_chunk &public_key,
                                                const uint32_t lock_until) {
  vector<uint8_t> lock_until_array(4);
  serializer<vector<uint8_t>::iterator>(lock_until_array.begin())
      .write_4_bytes_little_endian(lock_until);

  return operation::list{{lock_until_array},
                         {opcode::checklocktimeverify},
                         {opcode::drop},
                         {public_key},
                         {opcode::checksig}};
}

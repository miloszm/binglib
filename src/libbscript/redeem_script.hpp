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
#ifndef REDEEM_SCRIPT_HPP
#define REDEEM_SCRIPT_HPP

#include <binglib/bing_common.hpp>
//#include "src/common/bing_common.hpp"
#include <bitcoin/system.hpp>

class RedeemScript {
public:
  static libbitcoin::machine::operation::list
  to_pay_key_hash_pattern_with_lock(const libbitcoin::data_chunk &public_key,
                                    const uint32_t lock_until);
};

#endif

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
#ifndef OFFLINE_UNLOCK_TX_CREATOR_HPP
#define OFFLINE_UNLOCK_TX_CREATOR_HPP

#include <bitcoin/system.hpp>
#include <string>

using namespace std;

class OfflineUnlockTxCreator {
public:
    static string construct_unlock_tx(
            const string priv_key_wif,
            const string src_tx_id,
            const uint64_t satoshis_to_transfer,
            const uint32_t src_lock_until,
            const string target_addr
    );

    static string construct_unlock_tx(
            const bc::wallet::ec_private priv_key_ec,
            const string src_tx_id,
            const uint64_t satoshis_to_transfer,
            const uint32_t src_lock_until,
            const string target_addr
    );
};


#endif
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
#ifndef ONLINE_LOCK_TX_CREATOR_HPP
#define ONLINE_LOCK_TX_CREATOR_HPP

#include <binglib/bing_common.hpp>
//#include "src/common/bing_common.hpp"
#include <binglib/libb_client.hpp>
//#include <src/libbitcoinclient/libb_client.hpp>
#include <binglib/electrum_interface.hpp>
//#include "src/electrumclient/electrum_interface.hpp"
#include <bitcoin/system.hpp>
#include <string>
#include <binglib/bing_exception.hpp>
//#include "src/common/bing_exception.hpp"

using namespace std;

struct LockTxInfo {
    string locking_tx;
    string unlocking_info;
};

class OnlineLockTxCreator {
public:
    static LockTxInfo construct_p2sh_time_locking_tx_from_address(
            LibbClient &libb_client,
            const std::string src_addr,
            const std::string priv_key_wif,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );

    static LockTxInfo construct_p2sh_time_locking_tx_from_address(
            LibbClient &libb_client,
            const std::string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );

    static LockTxInfo construct_p2sh_time_locking_tx_from_address(
            ElectrumInterface &electrum_interface,
            const std::string src_addr,
            const std::string priv_key_wif,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );

    static LockTxInfo construct_p2sh_time_locking_tx_from_address(
            ElectrumInterface &electrum_interface,
            const std::string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );
private:
    static LockTxInfo do_construct_p2sh_time_locking_tx_from_address(
            const std::vector<libbitcoin::chain::output_point>& utxos,
            uint64_t available_funds,
            const std::string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );
};


#endif

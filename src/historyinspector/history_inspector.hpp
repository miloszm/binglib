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
#ifndef BINGDIYI_HISTORY_INSPECTOR_HPP
#define BINGDIYI_HISTORY_INSPECTOR_HPP

#include <binglib/wallet_state.hpp>
//#include "src/walletstate/wallet_state.hpp"
#include <binglib/libb_client.hpp>
//#include <src/libbitcoinclient/libb_client.hpp>
#include <bitcoin/system.hpp>


using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;
#include <string>

using namespace std;

struct TxBalanceInput {
    string funding_tx;
    int funding_idx;
    uint64_t value;
    bool in_wallet;
    string address;
};

struct TxBalanceOutput {
    string address;
    int script_kind;
    uint64_t value;
    bool in_wallet;
};

struct TxBalance {
    string tx_id;
    vector<TxBalanceInput> inputs;
    vector<TxBalanceOutput> outputs;
};

struct TxWalletImpact {
    int64_t balance_delta;
    bool is_p2sh;
    uint64_t funding_amount;
    string funding_address;
};

class HistoryInspector {
  public:
    HistoryInspector(bool is_testnet, XElectrumInterface &electrum_api_client, WalletState &wallet_state);
    virtual ~HistoryInspector();

    uint64_t calculate_total_balance(bool unconfirmed_only = false);
    uint64_t calculate_confirmed_balance();
    uint64_t calculate_address_balance(const string &address, bool unconfirmed_only = false);
    TxWalletImpact calculate_tx_wallet_impact(const string &tx_id);
    int64_t unconfirmed_txs_wallet_impact();
    void create_history_view_rows(bool bulk);
    void scan_balances();
    void do_addresses_subscriptions();
    void do_addresses_subscriptions_bulk();
    void clear_caches();
    void clear_caches_for_address(const string& address);

  private:
    bool is_testnet_;
    WalletState &wallet_state_;
    XElectrumInterface &electrum_api_client_;

  private:
    void analyse_tx_balances(string tx_id, vector<TxBalance> &balance_items);
    static uint64_t calc_address_balance(const string &address,
                                         vector<TxBalance> &balance_items);
    static header hex_2_header(string tx_hex);
    wallet::payment_address::list get_addresses(output &o);
    void append_funding_txs(string txid, vector<string>& txids);
};

#endif

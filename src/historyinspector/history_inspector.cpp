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
#include "history_inspector.hpp"
//#include <binglib/address_converter.hpp>
#include "src/utility/address_converter.hpp"

using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;

HistoryInspector::HistoryInspector(bool is_testnet,
                                   XElectrumInterface &electrum_api_client,
                                   WalletState &wallet_state)
    : is_testnet_(is_testnet), electrum_api_client_(electrum_api_client),
      wallet_state_(wallet_state) {}

HistoryInspector::~HistoryInspector() {}

void HistoryInspector::clear_caches() {
    wallet_state_.clear_caches();
}

void HistoryInspector::clear_caches_for_address(const string& address) {
    wallet_state_.clear_caches_for_address(address);
}


uint64_t HistoryInspector::calculate_total_balance(bool unconfirmed_only) {
    uint64_t balance{0};
    for (const string &address : wallet_state_.get_addresses()) {
        balance += calculate_address_balance(address, unconfirmed_only);
    }
    return balance;
}

uint64_t HistoryInspector::calculate_address_balance(const string &address, bool unconfirmed_only) {
    vector<AddressHistoryItem> history_items;

    wallet_state_.get_history(electrum_api_client_, address, history_items);

    vector<TxBalance> balance_items;

    for (AddressHistoryItem &item : history_items) {
        if (!unconfirmed_only || item.height <= 0)
            analyse_tx_balances(item.txid, balance_items);
    }

    uint64_t balance = calc_address_balance(address, balance_items);

    return balance;
}

void HistoryInspector::scan_balances() {
    map<string, uint64_t> address_to_balance;
    for (auto address : wallet_state_.get_addresses()) {
        address_to_balance[address] = calculate_address_balance(address);
    }
    wallet_state_.push_balance_update(address_to_balance);
}

void HistoryInspector::do_addresses_subscriptions() {
    map<string, string> address_to_historyhash;
    for (auto address : wallet_state_.get_addresses()) {
        wallet_state_.subscribe_address(electrum_api_client_, address);
        address_to_historyhash[address] = "";
    }
    wallet_state_.push_historyhash_update(address_to_historyhash);
}

void HistoryInspector::do_addresses_subscriptions_bulk() {
    map<string, string> address_to_historyhash;
    for (auto address : wallet_state_.get_addresses()) {
        address_to_historyhash[address] = "";
    }
    wallet_state_.subscribe_address_bulk(electrum_api_client_, wallet_state_.get_addresses());
    wallet_state_.push_historyhash_update(address_to_historyhash);
}

wallet::payment_address::list HistoryInspector::get_addresses(output &o) {
    if (is_testnet_) {
        return o.addresses(wallet::payment_address::testnet_p2kh,
                           wallet::payment_address::testnet_p2sh);
    } else {
        return o.addresses(wallet::payment_address::mainnet_p2kh,
                           wallet::payment_address::mainnet_p2sh);
    }
}

void HistoryInspector::analyse_tx_balances(string tx_id,
                                           vector<TxBalance> &balance_items) {
    chain::transaction tx =
        wallet_state_.get_transaction(electrum_api_client_, tx_id);
    vector<TxBalanceInput> balance_inputs;
    vector<TxBalanceOutput> balance_outputs;
    for (auto &i : tx.inputs()) {
        string funding_tx_id = encode_hash(i.previous_output().hash());
        int funding_idx = i.previous_output().index();
        chain::transaction funding_tx =
            wallet_state_.get_transaction(electrum_api_client_, funding_tx_id);
        auto &previous_output = funding_tx.outputs().at(funding_idx);
        wallet::payment_address::list axx = get_addresses(previous_output);
        if (axx.size() > 0) {
            wallet::payment_address &ax = axx.front();
            bool is_in_wallet = wallet_state_.is_in_wallet(ax.encoded());
            TxBalanceInput balance_input{funding_tx_id, funding_idx,
                                         previous_output.value(), is_in_wallet, ax.encoded()};
            balance_inputs.push_back(balance_input);
        } else {
            TxBalanceInput balance_input{funding_tx_id, funding_idx,
                                         previous_output.value(), false, ""};
            balance_inputs.push_back(balance_input);
        }
    };
    for (auto &o : tx.outputs()) {
        wallet::payment_address::list axx = get_addresses(o);
        if (axx.size() > 0) {
            wallet::payment_address &ax = axx.front();
            bool is_in_wallet = wallet_state_.is_in_wallet(ax.encoded());
            TxBalanceOutput balance_output{
                ax.encoded(), static_cast<int>(o.script().pattern()), o.value(),
                is_in_wallet};
            balance_outputs.push_back(balance_output);
        } else {
            TxBalanceOutput balance_output{"", -1, o.value(), false};
            balance_outputs.push_back(balance_output);
        }
    };
    TxBalance tx_balance{tx_id, balance_inputs, balance_outputs};
    balance_items.push_back(tx_balance);
}

/**
 * note: balance_items have to be in order of real time appearance of corresponding transactions
 */
uint64_t
HistoryInspector::calc_address_balance(const string &address,
                                       vector<TxBalance> &balance_items) {
    uint64_t balance{0};
    int cur_pos{0};
    for (TxBalance &balance_item : balance_items) {
        for (int oidx = 0; oidx < balance_item.outputs.size(); ++oidx) {
            TxBalanceOutput &o = balance_item.outputs[oidx];
            if (o.address == address) {
                balance += o.value;
                string cur_tx = balance_item.tx_id;
                int cur_idx = oidx;
                for (auto j = cur_pos + 1; j < balance_items.size(); ++j) {
                    TxBalance &balance_item2 = balance_items[j];
                    for (TxBalanceInput &i : balance_item2.inputs) {
                        if (i.funding_tx == cur_tx &&
                            i.funding_idx == cur_idx) {
                            balance -= i.value;
                        }
                    }
                }
            }
        }
        ++cur_pos;
    }
    return balance;
}

void HistoryInspector::append_funding_txs(string txid, vector<string>& txids) {
    chain::transaction tx =
            wallet_state_.get_transaction(electrum_api_client_, txid);
    for (auto &i : tx.inputs()) {
        string funding_tx_id = encode_hash(i.previous_output().hash());
        if (!funding_tx_id.empty()){
            txids.push_back(funding_tx_id);
        }
    };
}

TxWalletImpact HistoryInspector::calculate_tx_wallet_impact(const string &tx_id) {
    vector<TxBalance> balance_items;
    analyse_tx_balances(tx_id, balance_items);
    uint64_t sum_from_wallet_inputs{0};
    uint64_t sum_to_wallet_outputs{0};
    uint64_t total_in{0};
    uint64_t total_out{0};
    bool is_p2sh{false};
    string funding_address;
    for (const TxBalance &balance_item : balance_items) {
        for (const TxBalanceInput &i : balance_item.inputs) {
            bool inside = i.in_wallet;
            if (inside) {
                sum_from_wallet_inputs += i.value;
                funding_address = i.address;
            }
            total_in += i.value;
        }
        for (const TxBalanceOutput &o : balance_item.outputs) {
            bool inside = o.in_wallet;
            if (inside)
                sum_to_wallet_outputs += o.value;
            if (o.script_kind == static_cast<int>(script_pattern::pay_script_hash))
                is_p2sh = true;
            total_out += o.value;
        }
    }
    uint64_t fee = total_in - total_out;
    int64_t delta = static_cast<int64_t>(sum_to_wallet_outputs - sum_from_wallet_inputs);
    uint64_t funding_amount = sum_from_wallet_inputs - sum_to_wallet_outputs - fee;
    return TxWalletImpact{
        delta,
        is_p2sh,
        funding_amount,
        funding_address
    };
}

int64_t HistoryInspector::unconfirmed_txs_wallet_impact() {
    vector<TransactionInfo> sorted_txs =
            wallet_state_.get_all_txs_sorted(electrum_api_client_);

    int64_t balance_impact{0};
    for (auto &tx_info : sorted_txs) {
        if (tx_info.height == 0) {
            auto &tx = tx_info.tx;
            string tx_id = encode_hash(tx.hash());
            TxWalletImpact impact = calculate_tx_wallet_impact(tx_id);
            balance_impact += impact.balance_delta;
        }
    }
    return balance_impact;
}

uint64_t HistoryInspector::calculate_confirmed_balance() {
    vector<TransactionInfo> sorted_txs =
            wallet_state_.get_all_txs_sorted(electrum_api_client_);

    uint64_t balance{0};
    for (auto &tx_info : sorted_txs) {
        if (tx_info.height != 0) {
            auto &tx = tx_info.tx;
            string tx_id = encode_hash(tx.hash());
            TxWalletImpact impact = calculate_tx_wallet_impact(tx_id);
            balance += impact.balance_delta;
        }
    }
    return balance;
}

void HistoryInspector::create_history_view_rows(bool bulk) {
    vector<HistoryViewRow> history_view_rows;

    vector<TransactionInfo> sorted_txs;
    if (bulk) {
        sorted_txs = wallet_state_.get_all_txs_sorted_bulk(electrum_api_client_);

        vector<string> funding_txids;
        for (auto &tx_info : sorted_txs) {
            auto &tx = tx_info.tx;
            string txid = encode_hash(tx.hash());
            append_funding_txs(txid, funding_txids);
        }
        wallet_state_.load_txs_bulk(electrum_api_client_, funding_txids);
    } else {
        sorted_txs = wallet_state_.get_all_txs_sorted(electrum_api_client_);
    }

    for (auto &tx_info : sorted_txs) {
        auto &tx = tx_info.tx;
        string tx_id = encode_hash(tx.hash());
        TxWalletImpact impact = calculate_tx_wallet_impact(tx_id);
        // todo: cache it
        // for the time being I skip timestamp as it takes too long
        // let's see if there is some quicker way to obtain it
        //        string header_hex = electrum_api_client_.getBlockHeader(tx_and_height.height);
        //        chain::header block_header = hex_2_header(header_hex);
        //        uint32_t timestamp = block_header.timestamp();
        uint32_t timestamp = 0;
        HistoryViewRow history_view_row{
            timestamp,
            tx_info.height,
            impact.balance_delta,
            tx_id,
            0,
            impact.is_p2sh,
            impact.funding_amount,
            impact.funding_address,
            tx_info.fresh};
        history_view_rows.push_back(history_view_row);
    }
    uint64_t balance{0};
    for (auto p = history_view_rows.rbegin(); p != history_view_rows.rend();
         ++p) {
        balance += p->balance_delta;
        p->balance = balance;
    }
    wallet_state_.push_history_update(history_view_rows);
}

chain::header HistoryInspector::hex_2_header(string header_hex) {
    chain::header header;
    data_chunk header_chunk;

    if (!decode_base16(header_chunk, header_hex)) {
        throw std::invalid_argument("could not decode raw hex header");
    }

    if (!header.from_data(header_chunk)) {
        throw std::invalid_argument("could not decode header");
    }

    return header;
}

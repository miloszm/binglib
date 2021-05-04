#include <binglib/bing_common.hpp>
#include "wallet_state.hpp"
#include <algorithm>
#include <binglib/address_converter.hpp>

using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;

WalletState::WalletState(vector<string> &addresses, map<string,AddressDerivationResult>& address_to_data)
    : addresses_(addresses), address_to_data_(address_to_data) {
    for (const string& address: addresses){
        string spkh = AddressConverter::base58_to_spkh_hex(address);
        spkh_2_address_[spkh] = address;
    }
}

WalletState::~WalletState() {}

vector<string> &WalletState::get_addresses() { return addresses_; }

map<string,AddressDerivationResult> &WalletState::get_address_to_data(){ return address_to_data_; }

bool WalletState::is_in_wallet(string address) {
    return std::find(addresses_.begin(), addresses_.end(), address) !=
           addresses_.end();
}

chain::transaction WalletState::hex_2_tx(string tx_hex) {
    chain::transaction tx;
    data_chunk tx_chunk;

    if (!decode_base16(tx_chunk, tx_hex)) {
        throw std::invalid_argument("could not decode raw hex transaction");
    }

    if (!tx.from_data(tx_chunk)) {
        throw std::invalid_argument("could not decode transaction");
    }

    return tx;
}

chain::transaction
WalletState::get_transaction(ElectrumInterface &electrum_api_client,
                             string txid) {
    string tx_hex = txid_2_txhex_cache_[txid];
    if (tx_hex.empty()) {
        tx_hex = electrum_api_client.getTransaction(txid);
        txid_2_txhex_cache_[txid] = tx_hex;
        ProgressEvent progress_event {0, 0, 1};
        push_progress_event(progress_event);
    }
    return hex_2_tx(tx_hex);
}

void WalletState::get_history(ElectrumInterface &electrum_api_client,
                              const string &address,
                              vector<AddressHistoryItem> &history_items) {
    if (address_2_history_cache_empty_[address]){
        return;
    }
    vector<AddressHistoryItem> address_history =
        address_2_history_cache_[address];
    if (address_history.empty()) {
        string address_spkh = AddressConverter::base58_to_spkh_hex(address);
        AddressHistory history = electrum_api_client.getHistory(address_spkh);
        ProgressEvent progress_event {1, static_cast<int>(history.size()), 0};
        push_progress_event(progress_event);
        for (const AddressHistoryItem &history_item : history) {
            AddressHistoryItem ahi{history_item.txid, history_item.height, true};
            history_items.push_back(ahi);
        }
        address_2_history_cache_[address] = history_items;
        if (history_items.empty()){
            address_2_history_cache_empty_[address] = true;
        }
    } else {
        for (const AddressHistoryItem &history_item : address_history) {
            AddressHistoryItem ahi{history_item.txid, history_item.height, false};
            history_items.push_back(ahi);
        }
    }
}

void WalletState::refresh_all_history(ElectrumInterface &electrum_api_client) {
    auto cmp = [](const AddressHistoryItem& a, const AddressHistoryItem& b) { return a.txid < b.txid; };
    std::set<AddressHistoryItem, decltype(cmp)> ahi_set(cmp);
    for (auto &address : addresses_) {
        vector<AddressHistoryItem> history_items;
        get_history(electrum_api_client, address, history_items);
        for (auto &history_item : history_items) {
            ahi_set.insert(history_item);
        }
    }
    all_history_.clear();
    for (auto ahi: ahi_set) {
        all_history_.push_back(ahi);
    }
}

vector<TransactionInfo>
WalletState::get_all_txs_sorted(ElectrumInterface &electrum_api_client) {
    refresh_all_history(electrum_api_client);
    std::sort(all_history_.begin(), all_history_.end(),
              [](const AddressHistoryItem &lhs, const AddressHistoryItem &rhs) {
                  if (lhs.height != rhs.height)
                      if (lhs.height == 0)
                          return true;
                      else if (rhs.height == 0)
                          return false;
                      else
                        return lhs.height > rhs.height;
                  else
                      return lhs.txid > rhs.txid;
              });

    vector<TransactionInfo> txs;
    for (const AddressHistoryItem &item : all_history_) {
        if (item.txid.empty()){
            throw std::invalid_argument("empty transaction id in get_all_txs_sorted"); // should never happen
        } else {
            transaction tx = get_transaction(electrum_api_client, item.txid);
            TransactionInfo transaction_info{tx, item.height, item.fresh};
            txs.push_back(transaction_info);
        }
    }

    return txs;
}

string WalletState::spkh_2_address(string spkh) {
    return spkh_2_address_[spkh];
}

void WalletState::subscribe_address(ElectrumInterface &electrum_api_client, const string& address) {
    string address_spkh = AddressConverter::base58_to_spkh_hex(address);
    electrum_api_client.scripthashSubscribe(address_spkh);
}

void WalletState::clear_caches() {
    txid_2_txhex_cache_.clear();
    address_2_history_cache_.clear();
    address_2_history_cache_empty_.clear();
    all_history_.clear();
}

void WalletState::clear_caches_for_address(const string& address) {
    all_history_.clear();
    address_2_history_cache_[address] = vector<AddressHistoryItem>();
    address_2_history_cache_empty_[address] = false;
}

vector<HistoryViewRow> WalletState::get_history_update() {
    return history_queue_.pop();
}

void WalletState::push_history_update(vector<HistoryViewRow>& history_rows) {
    history_queue_.push(history_rows);
}

map<string, uint64_t> WalletState::get_balance_update() {
    return balance_queue_.pop();
}

void WalletState::push_balance_update(map<string, uint64_t>& balance_map) {
    balance_queue_.push(balance_map);
}

map<string, string> WalletState::get_historyhash_update() {
    return historyhash_queue.pop();
}

void WalletState::push_historyhash_update(map<string, string>& historyhash_map) {
    historyhash_queue.push(historyhash_map);
}

void WalletState::subscribe_to_progress_events(ProgressCallback progress_callback) {
    progress_callbacks_.push_back(progress_callback);
}

void WalletState::push_progress_event(ProgressEvent progress_event) {
    for (auto c: progress_callbacks_) {
        c(progress_event);
    }
}

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
WalletState::get_transaction(ElectrumApiClient &electrum_api_client,
                             string txid) {
    string tx_hex = txid_2_txhex_cache_[txid];
    if (tx_hex.empty()) {
        tx_hex = electrum_api_client.getTransaction(txid);
        txid_2_txhex_cache_[txid] = tx_hex;
    }
    return hex_2_tx(tx_hex);
}

void WalletState::get_history(ElectrumApiClient &electrum_api_client,
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

void WalletState::refresh_all_history(ElectrumApiClient &electrum_api_client) {
    all_history_.clear();
    for (auto &address : addresses_) {
        vector<AddressHistoryItem> history_items;
        get_history(electrum_api_client, address, history_items);
        for (auto &history_item : history_items) {
            all_history_.push_back(history_item);
        }
    }
}

vector<TransactionInfo>
WalletState::get_all_txs_sorted(ElectrumApiClient &electrum_api_client) {
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
    auto last = std::unique(
        all_history_.begin(), all_history_.end(),
        [](const AddressHistoryItem &lhs, const AddressHistoryItem &rhs) {
            return lhs.txid == rhs.txid;
        });
    all_history_.erase(last, all_history_.end());

    vector<TransactionInfo> txs;
    for (const AddressHistoryItem &item : all_history_) {
        transaction tx = get_transaction(electrum_api_client, item.txid);
        TransactionInfo transaction_info{tx, item.height, item.fresh};
        txs.push_back(transaction_info);
    }

    return txs;
}

string WalletState::spkh_2_address(string spkh) {
    return spkh_2_address_[spkh];
}

string WalletState::subscribe_address(ElectrumApiClient &electrum_api_client, const string& address) {
    bool is_subscribed = address_2_subscribed_[address];
    if (!is_subscribed) {
        string address_spkh = AddressConverter::base58_to_spkh_hex(address);
        string history_hash = electrum_api_client.scripthashSubscribe(address_spkh);
        address_2_subscribed_[address] = true;
        return history_hash;
    }
    else return "";
}

void WalletState::clear_caches() {
    txid_2_txhex_cache_.clear();
    address_2_history_cache_.clear();
    address_2_history_cache_empty_.clear();
    address_2_subscribed_.clear();
    all_history_.clear();
}

void WalletState::clear_caches_for_address(const string& address) {
    all_history_.clear();
    address_2_history_cache_[address] = vector<AddressHistoryItem>();
    address_2_history_cache_empty_[address] = false;
}

vector<HistoryViewRow> WalletState::get_history_update() {
    history_queue_.pop();
}

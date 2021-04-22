#ifndef WALLET_STATE_HPP
#define WALLET_STATE_HPP

#include <binglib/electrum_api_client.hpp>
#include <binglib/bing_wallet.hpp>
#include <bitcoin/bitcoin.hpp>
#include "src/utility/blocking_queue.hpp"

using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;
#include <string>

using namespace std;

struct TransactionInfo {
    transaction tx;
    int height;
    bool fresh;
};

struct HistoryViewRow {
    uint32_t timestamp;
    int height;
    int64_t balance_delta;
    string tx_id;
    uint64_t balance;
    bool is_p2sh;
    uint64_t funding_amount;
    string funding_address;
    bool fresh;
};

class WalletState {
  public:
    WalletState(vector<string> &addresses, map<string,AddressDerivationResult>& address_to_data);
    virtual ~WalletState();
    vector<string> &get_addresses();
    map<string,AddressDerivationResult> &get_address_to_data();
    bool is_in_wallet(string address);
    transaction get_transaction(ElectrumApiClient &electrum_api_client,
                                string txid);
    void get_history(ElectrumApiClient &electrum_api_client,
                     const string &address,
                     vector<AddressHistoryItem> &history_items);
    vector<TransactionInfo>
    get_all_txs_sorted(ElectrumApiClient &electrum_api_client);
    string spkh_2_address(string spkh);
    string subscribe_address(ElectrumApiClient &electrum_api_client, const string& address);
    void clear_caches();
    void clear_caches_for_address(const string& address);
    vector<HistoryViewRow> get_history_update();

  private:
    vector<string> addresses_;
    map<string, string> spkh_2_address_;
    map<string, string> txid_2_txhex_cache_;
    map<string, vector<AddressHistoryItem>> address_2_history_cache_;
    map<string, bool> address_2_history_cache_empty_;
    vector<AddressHistoryItem> all_history_;
    map<string, bool> address_2_subscribed_;
    map<string, AddressDerivationResult> address_to_data_;
    blocking_queue<vector<HistoryViewRow>> history_queue_;

  private:
    static transaction hex_2_tx(string tx_hex);
    void refresh_all_history(ElectrumApiClient &electrum_api_client);
};

#endif

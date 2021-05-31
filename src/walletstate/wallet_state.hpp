#ifndef WALLET_STATE_HPP
#define WALLET_STATE_HPP

#include <binglib/bing_wallet.hpp>
//#include <src/wallet/bing_wallet.hpp>
#include <bitcoin/system.hpp>
#include <binglib/blocking_queue.hpp>
//#include "src/utility/blocking_queue.hpp"
#include <binglib/electrum_interface.hpp>
//#include "src/electrumclient/electrum_interface.hpp"

using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;
#include <string>
#include <functional>
using namespace std::placeholders;

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

struct ProgressEvent {
    int history_read_count;
    int tx_pending_count;
    int tx_read_count;
};

typedef std::function<void(ProgressEvent)> ProgressCallback;

class WalletState {
  public:
    WalletState(vector<string> &addresses, map<string,AddressDerivationResult>& address_to_data);
    virtual ~WalletState();
    vector<string> &get_addresses();
    map<string,AddressDerivationResult> &get_address_to_data();
    bool is_in_wallet(string address);
    transaction get_transaction(XElectrumInterface &electrum_api_client,
                                string txid);
    void get_history(XElectrumInterface &electrum_api_client,
                     const string &address,
                     vector<AddressHistoryItem> &history_items);
    vector<TransactionInfo>
    get_all_txs_sorted(XElectrumInterface &electrum_api_client);
    vector<TransactionInfo>
    get_all_txs_sorted_bulk(XElectrumInterface &electrum_api_client);
    string spkh_2_address(string spkh);
    void subscribe_address(XElectrumInterface &electrum_api_client, const string& address);
    void subscribe_address_bulk(XElectrumInterface &electrum_api_client, vector<string>& addresses);
    void clear_caches();
    void clear_caches_for_address(const string& address);
    vector<HistoryViewRow> get_history_update();
    void push_history_update(vector<HistoryViewRow>& history_rows);
    map<string, uint64_t> get_balance_update();
    void push_balance_update(map<string, uint64_t>& balance_map);
    map<string, string> get_historyhash_update();
    void push_historyhash_update(map<string, string>& historyhash_map);
    void subscribe_to_progress_events(ProgressCallback progress_callback);
    void clear_progress_events_subscriptions();
    void load_txs_bulk(XElectrumInterface &electrum_api_client, const vector<string>& txids);

  private:
    vector<string> addresses_;
    map<string, string> spkh_2_address_;
    map<string, string> txid_2_txhex_cache_;
    map<string, vector<AddressHistoryItem>> address_2_history_cache_;
    map<string, bool> address_2_history_cache_empty_;
    vector<AddressHistoryItem> all_history_;
    map<string, AddressDerivationResult> address_to_data_;
    blocking_queue<vector<HistoryViewRow>> history_queue_;
    blocking_queue<map<string, uint64_t>> balance_queue_;
    blocking_queue<map<string, string>> historyhash_queue;
    vector<ProgressCallback> progress_callbacks_;

  private:
    static transaction hex_2_tx(string tx_hex);
    void refresh_all_history(XElectrumInterface &electrum_api_client);
    void refresh_all_history_bulk(XElectrumInterface &electrum_api_client);
    void sort_all_history();
    void push_progress_event(ProgressEvent progress_event);
};

#endif

#ifndef ELECTRUM_API_CLIENT_HPP
#define ELECTRUM_API_CLIENT_HPP

#include <string>
#include "electrum_client.hpp"


using namespace std;

struct AddressHistoryItem {
    string txid;
    int height;
    bool fresh;
};


void address_history_item_from_json(const nlohmann::json& j, AddressHistoryItem& ahi);

typedef vector<AddressHistoryItem> AddressHistory;

void address_history_from_json(const nlohmann::json& j, AddressHistory& ah);

struct AddressBalance {
    long confirmed;
    long unconfirmed;
};

void address_balance_from_json(const nlohmann::json& j, AddressBalance& ab);

struct Utxo {
    uint32_t tx_pos;
    string tx_id;
    uint64_t value;
    int height;
};

void utxo_from_json(const nlohmann::json& j, Utxo& utxo);

void utxos_from_json(const nlohmann::json& j, vector<Utxo>& utxos);

class ElectrumApiClient {
public:
    ElectrumApiClient(ElectrumClient& client): client_(client){}

    void scripthashSubscribe(string scripthash);
    AddressHistory getHistory(string address);
    string getTransaction(string txid);
    AddressBalance getBalance(string address);
    string getBlockHeader(int height);
    vector<Utxo> getUtxos(string scripthash);
    double estimateFee(int wait_blocks);
    string broadcastTransaction(string txid);
    ElectrumMessage run_receiving_loop(){ return client_.run_receiving_loop(); }
    static bool is_scripthash_update(const ElectrumMessage& electrum_message);
private:
    ElectrumClient& client_;
    std::atomic<int> id_counter;
};

#endif
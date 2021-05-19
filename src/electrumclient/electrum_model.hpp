#ifndef ELECTRUM_MODEL_HPP
#define ELECTRUM_MODEL_HPP

#include <nlohmann/json.hpp>
#include <string>

using namespace std;
using json = nlohmann::json;

struct ElectrumRequest {
    string method;
    int id;
    vector<string> params;
};

struct ElectrumVersionRequest {
    string method;
    int id;
    string param1;
    vector<string> param2;
};

void electrum_request_to_json(nlohmann::json& j, const ElectrumRequest& r);

void electrum_request_to_json(nlohmann::json& j, const ElectrumVersionRequest& r);

struct ElectrumMessage {
    json message;
    string method;
    bool has_correlation_id;
    int correlation_id;
    vector<string> params;
    bool isNone() { return correlation_id == -1; }
};

bool is_scripthash_update(const ElectrumMessage& electrum_message);

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

struct ServerInfo {
    string host;
    string service;
    string version;
    string pruning;
};

void server_info_from_json(const nlohmann::json& j, ServerInfo& server_info);

void server_infos_from_json(const nlohmann::json& j, vector<ServerInfo>& server_infos);


#endif
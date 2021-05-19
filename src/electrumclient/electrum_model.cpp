#include "electrum_model.hpp"

void electrum_request_to_json(nlohmann::json &j, const ElectrumRequest &r) {
    j = nlohmann::json{{"jsonrpc", "2.0"},
                       {"method", r.method},
                       {"id", r.id},
                       {"params", r.params}};
}

void electrum_request_to_json(nlohmann::json &j, const ElectrumVersionRequest &r) {
    j = nlohmann::json{{"jsonrpc", "2.0"},
                       {"method", r.method},
                       {"id", r.id},
                       {"params", {r.param1, r.param2}}};
}

bool is_scripthash_update(const ElectrumMessage& electrum_message){
    return electrum_message.method == "blockchain.scripthash.subscribe";
}

void address_history_item_from_json(const nlohmann::json& j, AddressHistoryItem& ahi) {
    j.at("tx_hash").get_to(ahi.txid);
    j.at("height").get_to(ahi.height);
}

void address_history_from_json(const nlohmann::json& j, AddressHistory& ah) {
    auto items = j.items();
    for (auto i = items.begin(); i != items.end(); ++i){
        AddressHistoryItem ahi;
        address_history_item_from_json(i.value(), ahi);
        ah.push_back(ahi);
    }
}

void address_balance_from_json(const nlohmann::json& j, AddressBalance& ab) {
    j.at("confirmed").get_to(ab.confirmed);
    j.at("unconfirmed").get_to(ab.unconfirmed);
}

void utxo_from_json(const nlohmann::json& j, Utxo& utxo) {
    j.at("tx_pos").get_to(utxo.tx_pos);
    j.at("value").get_to(utxo.value);
    j.at("tx_hash").get_to(utxo.tx_id);
    j.at("height").get_to(utxo.height);
}

void utxos_from_json(const nlohmann::json& j, vector<Utxo>& utxos) {
    auto items = j.items();
    for (auto i = items.begin(); i != items.end(); ++i){
        Utxo u;
        utxo_from_json(i.value(), u);
        utxos.push_back(u);
    }
}


// ["107.150.45.210",
// "e.anonyhost.org",
// ["v1.0", "p10000", "t", "s995"]]
void server_info_from_json(const nlohmann::json& j, ServerInfo& server_info) {
    auto items = j.items();
    int i = 0;
    for (auto p = items.begin(); p != items.end(); ++p, ++i) {
        if (i == 1){
            server_info.host = p.value();
        } else if (i == 2){
            auto elem_3_items = p.value().items();
            int j = 0;
            for (auto r = elem_3_items.begin(); r != elem_3_items.end(); ++r, ++j) {
                if (j == 0){
                    server_info.version = r.value();
                } else if (j == 1){
                    string service = r.value();
                    if (!service.empty()) {
                        server_info.service = service.substr(1);
                    }
                }
            }
        }
    }
}

void server_infos_from_json(const nlohmann::json& j, vector<ServerInfo>& server_infos) {
    auto items = j.items();
    for (auto i = items.begin(); i != items.end(); ++i){
        ServerInfo si;
        server_info_from_json(i.value(), si);
        server_infos.push_back(si);
    }
}

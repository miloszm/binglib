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

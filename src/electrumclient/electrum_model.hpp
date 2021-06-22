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
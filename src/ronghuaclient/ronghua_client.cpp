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
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;
using json = nlohmann::json;
using namespace std;
using json = nlohmann::json;
#include "src/electrumclient/electrum_model.hpp"
#include "src/ronghuaclient/ronghua_client.hpp"

RonghuaClient::RonghuaClient()
:   client_(nullptr),
    io_context_(new boost::asio::io_context()),
    ctx_(new boost::asio::ssl::context(boost::asio::ssl::context::sslv23)),
    id_counter(0),
    interrupt_requested_(false)
    {}

RonghuaClient::~RonghuaClient() {
    delete io_context_;
    delete ctx_;
}

bool RonghuaClient::init(string hostname, string service,
                          string cert_file_path) {
    tcp::resolver resolver(*io_context_);
    boost::system::error_code ec;
    endpoints_ = resolver.resolve(hostname, service, ec);
    if (ec.failed()){
        cout << "resolver error: " << ec.value() << " " << ec.message() << "\n";
        return false;
    }

    ctx_->load_verify_file(cert_file_path);
    client_.reset(new RonghuaSocketClient(*io_context_, *ctx_, endpoints_, interrupt_requested_, electrum_error_callbacks_));
    io_context_->run();
    client_.get()->run_receiving_loop();
//    client_.get()->prepare_connection.lock();
    return true;
}

void RonghuaClient::stop() {
//    client_.get()->prepare_connection.unlock();
    client_.get()->stop();
    io_context_->stop();
}

json RonghuaClient::send_request(json json_request, int id) {
    unique_lock<mutex> lock(mutex_);
    client_->send_request(json_request);
    return client_->receive_response(id);
}

int RonghuaClient::send_request_no_response(json json_request, int id) {
    unique_lock<mutex> lock(mutex_);
    client_->send_request(json_request);
    return id;
}

void RonghuaClient::send_request_eat_response(json json_request, int id) {
    client_->send_request(json_request);
    client_->eat_response(id);
}

void RonghuaClient::process_exception(exception& e, nlohmann::json response, const string& msg) {
    string error_message;
    if (response.empty()) {
        error_message = msg + " failed: " + e.what();
    }
    else {
        try {
            error_message = response.at("error").at("message");
        } catch (exception &e) {
            error_message = msg + " failed: " + response.dump() + " " + e.what();
        }
    }
    throw std::invalid_argument(error_message);
}

AddressHistory RonghuaClient::getHistory(string address) {
    vector<string> av{address};
    ElectrumRequest request{"blockchain.scripthash.get_history", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    cout << "getHistory " << address << "\n";
    AddressHistory address_history;
    json json_response;
    try {
        json_response = send_request(json_request, id_counter);
        address_history_from_json(json_response["result"], address_history);
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.scripthash.get_history");
    }
    return address_history;
}

vector<AddressHistory> RonghuaClient::getHistoryBulk(vector<string> addresses, vector<ProgressCallback>& progress_callbacks) {
    cout << "getHistoryBulk " << addresses.size() << "\n";
    vector<AddressHistory> histories;
    const int target_count = addresses.size();
    const int chunk_factor = 25;
    int cur_pos = 0;
    while (cur_pos < target_count) {
        vector<string> chunk_ids(addresses.begin() + cur_pos, addresses.begin() + min(cur_pos + chunk_factor, target_count));
        doGetHistoryBulk(chunk_ids, histories);
        cur_pos = histories.size();
    }
    return histories;
}

void RonghuaClient::doGetHistoryBulk(const vector<string>& addresses, vector<AddressHistory>& histories) {
    cout << "doGetHistoryBulk " << addresses.size() << "\n";
    vector<int> ids;
    for (string address: addresses) {
        vector<string> av{address};
        ElectrumRequest request{"blockchain.scripthash.get_history", ++id_counter, av};
        json json_request;
        electrum_request_to_json(json_request, request);
        int id = send_request_no_response(json_request, id_counter);
        ids.push_back(id);
    }
    vector<json> responses = client_->receive_response_bulk(ids);
    int i = 0;
    for (auto r: responses) {
        try {
            AddressHistory ah;
            address_history_from_json(r["result"], ah);
            histories.push_back(ah);
            cout << "getHistoryBulk " << addresses.at(i) << "\n";
        } catch(exception& e){
            process_exception(e, r, "blockchain.scripthash.get_history");
        }
        ++i;
    }
}

void RonghuaClient::scripthashSubscribe(string scripthash) {
    vector<string> scripthashv{scripthash};
    ElectrumRequest request{"blockchain.scripthash.subscribe", ++id_counter, scripthashv};
    json json_request;
    electrum_request_to_json(json_request, request);
    send_request_eat_response(json_request, id_counter);
}

void RonghuaClient::scripthashSubscribeBulk(vector<string> scripthashes) {
    throw std::invalid_argument("scripthashSubscribeBulk not implemented");
}

vector<ServerInfo> RonghuaClient::getServerPeers() {
    throw std::invalid_argument("getServerPeers not implemented");
}

vector<Utxo> RonghuaClient::getUtxos(string scripthash) {
    vector<string> scripthashv{scripthash};
    ElectrumRequest request{"blockchain.scripthash.listunspent", ++id_counter, scripthashv};
    json json_request;
    electrum_request_to_json(json_request, request);
    cout << "getUtxos " << scripthash << "\n";
    vector<Utxo> utxos;
    json json_response;
    try {
        json_response = send_request(json_request, id_counter);
        if (json_response["result"].is_null()){
            utxos = vector<Utxo>();
        } else {
            utxos_from_json(json_response.at("result"), utxos);
        }
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.scripthash.listunspent");
    }
    return utxos;
}

string RonghuaClient::getTransaction(string txid) {
    cout << "getTransaction " << txid << "\n";
    if (txid.empty()){
        throw std::invalid_argument("getTransaction txid is empty");
    }
    vector<string> txidv{txid};
    ElectrumRequest request{"blockchain.transaction.get", ++id_counter, txidv};
    json json_request;
    electrum_request_to_json(json_request, request);
    string response;
    json json_response;
    try {
        json_response = send_request(json_request, id_counter);
        response =json_response.at("result");
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.transaction.get");
    }
    return response;
}

vector<string> RonghuaClient::getTransactionBulk(vector<string> txids, vector<ProgressCallback>& progress_callbacks) {
    cout << "getTransactionBulk " << txids.size() << "\n";
    vector<string> txhexes;
    const int target_count = txids.size();
    const int chunk_factor = 100;
    int cur_pos = 0;
    while (cur_pos < target_count) {
        vector<string> chunk_ids(txids.begin() + cur_pos, txids.begin() + min(cur_pos + chunk_factor, target_count));
        doGetTransactionBulk(chunk_ids, txhexes);
        cur_pos = txhexes.size();
    }
    return txhexes;
}

void RonghuaClient::doGetTransactionBulk(const vector<string>& txids, vector<string>& txhexes) {
    vector<int> ids;
    for (string txid: txids){
        vector<string> txidv{txid};
        ElectrumRequest request{"blockchain.transaction.get", ++id_counter, txidv};
        json json_request;
        electrum_request_to_json(json_request, request);
        int id = send_request_no_response(json_request, id_counter);
        ids.push_back(id);
    }
    vector<json> responses = client_->receive_response_bulk(ids);
    int i = 0;
    for (auto r: responses) {
        try {
            string response = r.at("result");
            txhexes.push_back(response);
            //cout << "doGetTransactionBulk " << txids.at(i) << "\n";
        } catch(exception& e){
            process_exception(e, r, "blockchain.transaction.get");
        }
        ++i;
    }
}

AddressBalance RonghuaClient::getBalance(string address) {
    vector<string> av{address};
    ElectrumRequest request{"blockchain.scripthash.get_balance", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    AddressBalance address_balance;
    json json_response;
    try {
        json_response = send_request(json_request, id_counter);
        address_balance_from_json(json_response["result"], address_balance);
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.scripthash.get_balance");
    }
    return address_balance;
}

string RonghuaClient::getBlockHeader(int height) {
    vector<string> av{to_string(height), "0"};
    ElectrumRequest request{"blockchain.block.header", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    string response;
    json json_response;
    try {
        json_response = send_request(json_request, id_counter);
        response = json_response.at("result");
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.block.header");
    }
    return response;
}

void RonghuaClient::ping(){
    vector<string> av;
    ElectrumRequest request{"server.ping", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    send_request_eat_response(json_request, id_counter);
}

double RonghuaClient::estimateFee(int wait_blocks) {
    vector<string> av{to_string(wait_blocks)};
    ElectrumRequest request{"blockchain.estimatefee", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    double response;
    json json_response;
    try {
        json_response = send_request(json_request, id_counter);
        response = json_response.at("result");
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.estimatefee");
    }
    return response;
}

string RonghuaClient::broadcastTransaction(string tx_hex) {
    vector<string> tx_hexv{tx_hex};
    ElectrumRequest request{"blockchain.transaction.broadcast", ++id_counter, tx_hexv};
    json json_request;
    electrum_request_to_json(json_request, request);
    cout << "broadcastTransaction\n";
    string response;
    json json_response;
    try {
        json_response = send_request(json_request, id_counter);
        response = json_response.at("result");
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.transaction.broadcast");
    }
    return response;
}

vector<string> RonghuaClient::getVersion(string client_name, vector<string> protocol_min_max) {
    ElectrumVersionRequest request{"server.version", ++id_counter, client_name, protocol_min_max};
    json json_request;
    electrum_request_to_json(json_request, request);
    cout << "getVersion\n";
    cout << json_request.dump(4) << "\n";
    vector<string> response;
    json json_response;
    try {
        json_response = send_request(json_request, id_counter);
        cout << json_response.dump(4) << "\n";
        json_response.at("result").get_to(response);
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.transaction.broadcast");
    }
    return response;
}

void RonghuaClient::do_interrupt() {
    interrupt_requested_ = true;
    client_->do_interrupt();
}

ElectrumMessage RonghuaClient::get_subscription_event() {
    return client_->get_subscription_event();
}

void RonghuaClient::subscribe_to_error_events(ElectrumErrorCallback error_callback) {
    electrum_error_callbacks_.push_back(error_callback);
}

void RonghuaClient::clear_error_events_subscriptions() {
    electrum_error_callbacks_.clear();
}

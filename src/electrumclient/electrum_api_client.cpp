#include <iostream>
#include "electrum_api_client.hpp"

using json = nlohmann::json;
using namespace std;


void ElectrumApiClient::init(string hostname, string service, string certification_file_path) {
    hostname_ = hostname;
    service_ = service;
    certification_file_path_ = certification_file_path;
    client_->init(hostname_, service_, certification_file_path_);
}

void ElectrumApiClient::process_exception(exception& e, nlohmann::json response, const string& msg) {
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

AddressHistory ElectrumApiClient::getHistory(string address){
    vector<string> av{address};
    ElectrumRequest request{"blockchain.scripthash.get_history", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    cout << "getHistory " << address << "\n";
    AddressHistory address_history;
    json json_response;
    try {
        json_response = client_->send_request(json_request, id_counter);
        address_history_from_json(json_response["result"], address_history);
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.scripthash.get_history");
    }
    return address_history;
}

void ElectrumApiClient::scripthashSubscribe(string scripthash) {
    vector<string> scripthashv{scripthash};
    ElectrumRequest request{"blockchain.scripthash.subscribe", ++id_counter, scripthashv};
    json json_request;
    electrum_request_to_json(json_request, request);
    client_->send_request_eat_response(json_request, id_counter);
}

vector<Utxo> ElectrumApiClient::getUtxos(string scripthash) {
    vector<string> scripthashv{scripthash};
    ElectrumRequest request{"blockchain.scripthash.listunspent", ++id_counter, scripthashv};
    json json_request;
    electrum_request_to_json(json_request, request);
    cout << "getUtxos " << scripthash << "\n";
    vector<Utxo> utxos;
    json json_response;
    try {
        json_response = client_->send_request(json_request, id_counter);
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

string ElectrumApiClient::getTransaction(string txid){
    vector<string> txidv{txid};
    ElectrumRequest request{"blockchain.transaction.get", ++id_counter, txidv};
    json json_request;
    electrum_request_to_json(json_request, request);
    string response;
    json json_response;
    try {
        json_response = client_->send_request(json_request, id_counter);
        response =json_response.at("result");
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.transaction.get");
    }
    return response;
}

AddressBalance ElectrumApiClient::getBalance(string address){
    vector<string> av{address};
    ElectrumRequest request{"blockchain.scripthash.get_balance", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    AddressBalance address_balance;
    json json_response;
    try {
        json_response = client_->send_request(json_request, id_counter);
        address_balance_from_json(json_response["result"], address_balance);
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.scripthash.get_balance");
    }
    return address_balance;
}

string ElectrumApiClient::getBlockHeader(int height){
    vector<string> av{to_string(height), "0"};
    ElectrumRequest request{"blockchain.block.header", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    string response;
    json json_response;
    try {
        json_response = client_->send_request(json_request, id_counter);
        response = json_response.at("result");
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.block.header");
    }
    return response;
}

double ElectrumApiClient::estimateFee(int wait_blocks){
    vector<string> av{to_string(wait_blocks)};
    ElectrumRequest request{"blockchain.estimatefee", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    double response;
    json json_response;
    try {
        json_response = client_->send_request(json_request, id_counter);
        response = json_response.at("result");
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.estimatefee");
    }
    return response;
}

string ElectrumApiClient::broadcastTransaction(string tx_hex){
    vector<string> tx_hexv{tx_hex};
    ElectrumRequest request{"blockchain.transaction.broadcast", ++id_counter, tx_hexv};
    json json_request;
    electrum_request_to_json(json_request, request);
    cout << "broadcastTransaction\n";
    string response;
    json json_response;
    try {
        json_response = client_->send_request(json_request, id_counter);
        response = json_response.at("result");
    } catch(exception& e){
        process_exception(e, json_response, "blockchain.transaction.broadcast");
    }
    return response;
}

bool ElectrumApiClient::is_scripthash_update(const ElectrumMessage& electrum_message){
    return electrum_message.method == "blockchain.scripthash.subscribe";
}

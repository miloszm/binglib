#include <iostream>
#include "electrum_api_client.hpp"

using json = nlohmann::json;
using namespace std;


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


AddressHistory ElectrumApiClient::getHistory(string address){
    vector<string> av{address};
    ElectrumRequest request{"blockchain.scripthash.get_history", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    json json_response = client_.send_request(json_request, id_counter);
    AddressHistory address_history;
    address_history_from_json(json_response["result"], address_history);
    cout << "getHistory " << address << "\n";
    return address_history;
}


string ElectrumApiClient::scripthashSubscribe(string scripthash) {
    vector<string> scripthashv{scripthash};
    ElectrumRequest request{"blockchain.scripthash.subscribe", ++id_counter, scripthashv};
    json json_request;
    electrum_request_to_json(json_request, request);
    json json_response = client_.send_request(json_request, id_counter);
    cout << "scripthashSubscribe " << scripthash << "\n";
    string response;
    try {
        if (json_response["result"].is_null()){
            response = "";
        } else {
            response = json_response.at("result");
        }
    } catch(exception& e){
        string error_message;
        try {
            error_message = json_response.at("error").at("message");
        } catch(exception& e){
            error_message = "blockchain.scripthash.subscribe failed: " + json_response.dump() + " " + e.what();
        }
        throw std::invalid_argument(error_message);
    }
    return response;
}


string ElectrumApiClient::getTransaction(string txid){
    vector<string> txidv{txid};
    ElectrumRequest request{"blockchain.transaction.get", ++id_counter, txidv};
    json json_request;
    electrum_request_to_json(json_request, request);
    json json_response = client_.send_request(json_request, id_counter);
    cout << "getTransaction " << txid << "\n";
    string response;
    try {
        response =json_response.at("result");
    } catch(exception& e){
        throw std::invalid_argument("Electrum blockchain.transaction.get failed: " + json_response.dump() + " " + e.what());
    }
    return response;
}


AddressBalance ElectrumApiClient::getBalance(string address){
    vector<string> av{address};
    ElectrumRequest request{"blockchain.scripthash.get_balance", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    json json_response = client_.send_request(json_request, id_counter);
    AddressBalance address_balance;
    address_balance_from_json(json_response["result"], address_balance);
    cout << "getBalance " << address << "\n";
    return address_balance;
}


string ElectrumApiClient::getBlockHeader(int height){
    vector<string> av{to_string(height), "0"};
    ElectrumRequest request{"blockchain.block.header", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    json json_response = client_.send_request(json_request, id_counter);
    cout << "getBlockHeader " << height << "\n";
    string response;
    try {
        response = json_response.at("result");
    } catch(exception& e){
        throw std::invalid_argument("Electrum blockchain.block.header failed: " + json_response.dump() + " " + e.what());
    }
    return response;
}


double ElectrumApiClient::estimateFee(int wait_blocks){
    vector<string> av{to_string(wait_blocks)};
    ElectrumRequest request{"blockchain.estimatefee", ++id_counter, av};
    json json_request;
    electrum_request_to_json(json_request, request);
    json json_response = client_.send_request(json_request, id_counter);
    cout << "estimateFee " << "\n";
    double response;
    try {
        response = json_response.at("result");
    } catch(exception& e){
        throw std::invalid_argument("Electrum blockchain.estimatefee failed: " + json_response.dump() + " " + e.what());
    }
    return response;
}


string ElectrumApiClient::broadcastTransaction(string tx_hex){
    vector<string> tx_hexv{tx_hex};
    ElectrumRequest request{"blockchain.transaction.broadcast", ++id_counter, tx_hexv};
    json json_request;
    electrum_request_to_json(json_request, request);
    json json_response = client_.send_request(json_request, id_counter);
    cout << "broadcastTransaction\n";
    string response;
    try {
        response = json_response.at("result");
    } catch(exception& e){
        string error_message;
        try {
            error_message = json_response.at("error").at("message");
        } catch(exception& e){
            error_message = "bloackchain broadcast failed: " + json_response.dump() + " " + e.what();
        }
        throw std::invalid_argument(error_message);
    }
    return response;
}



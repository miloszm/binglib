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
#ifndef RONGHUA_CLIENT_HPP
#define RONGHUA_CLIENT_HPP

#include <binglib/ronghua_socket_client.hpp>
//#include "src/ronghuaclient/ronghua_socket_client.hpp"
#include <binglib/electrum_model.hpp>
//#include "src/electrumclient/electrum_model.hpp"
#include <string>

using namespace std;
using boost::asio::ip::tcp;


class RonghuaClient : public ElectrumInterface {
public:
    RonghuaClient();
    virtual ~RonghuaClient();

    bool init(string hostname, string service, string certificationFilePath) override;
    void scripthashSubscribe(string scripthash) override;
    void scripthashSubscribeBulk(vector<string> scripthashes) override;
    AddressHistory getHistory(string address) override;
    vector<AddressHistory> getHistoryBulk(vector<string> addresses, vector<ProgressCallback>& progress_callbacks) override;
    string getTransaction(string txid) override;
    vector<string> getTransactionBulk(vector<string> txids, vector<ProgressCallback>& progress_callbacks) override;
    AddressBalance getBalance(string address) override;
    string getBlockHeader(int height) override;
    vector<Utxo> getUtxos(string scripthash) override;
    vector<ServerInfo> getServerPeers() override;
    double estimateFee(int wait_blocks) override;
    string broadcastTransaction(string txid) override;
    void ping() override;
    vector<string> getVersion(string client_name, vector<string> protocol_min_max) override;
    ElectrumMessage get_subscription_event() override;
    void do_interrupt() override;
    void stop() override;
    void subscribe_to_error_events(ElectrumErrorCallback error_callback) override;
    void clear_error_events_subscriptions() override;
    nlohmann::json send_request(nlohmann::json json_request, int id);

private:
    unique_ptr<RonghuaSocketClient> client_;
    boost::asio::io_context* io_context_;
    boost::asio::ssl::context* ctx_;
    tcp::resolver::results_type endpoints_;
    std::mutex mutex_;
    std::atomic<int> id_counter;
    std::atomic<bool> interrupt_requested_;
    vector<ElectrumErrorCallback> electrum_error_callbacks_;

    int send_request_no_response(json json_request, int id);
    void send_request_eat_response(nlohmann::json json_request, int id);
    void process_exception(exception& e, nlohmann::json response, const string& msg);
    void doGetHistoryBulk(const vector<string>& addresses, vector<AddressHistory>& histories);
    void doGetTransactionBulk(const vector<string>& txids, vector<string>& txhexes);
};

#endif

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
#ifndef ELECTRUM_INTERFACE_HPP
#define ELECTRUM_INTERFACE_HPP
#include <string>
#include "binglib/electrum_model.hpp"
//#include "src/electrumclient/electrum_model.hpp"

using namespace std;

struct ElectrumErrorEvent {
    int error_code;
    string error_message;
};

typedef std::function<void(ElectrumErrorEvent)> ElectrumErrorCallback;

struct ProgressEvent {
    int history_read_delta;
    int history_total;
    int txs_read_delta;
    int txs_total;
};

typedef std::function<void(ProgressEvent)> ProgressCallback;

class ElectrumInterface {
public:
    virtual bool init(string hostname, string service, string certification_file_path) = 0;
    virtual void scripthashSubscribe(string scripthash) = 0;
    virtual void scripthashSubscribeBulk(vector<string> scripthashes) = 0;
    virtual AddressHistory getHistory(string address) = 0;
    virtual vector<AddressHistory> getHistoryBulk(vector<string> addresses, vector<ProgressCallback>& progress_callbacks) = 0;
    virtual string getTransaction(string txid) = 0;
    virtual vector<string> getTransactionBulk(vector<string> txids, vector<ProgressCallback>& progress_callbacks) = 0;
    virtual AddressBalance getBalance(string address) = 0;
    virtual string getBlockHeader(int height) = 0;
    virtual vector<Utxo> getUtxos(string scripthash) = 0;
    virtual vector<ServerInfo> getServerPeers() = 0;
    virtual double estimateFee(int wait_blocks) = 0;
    virtual string broadcastTransaction(string txid) = 0;
    virtual void ping() = 0;
    virtual vector<string> getVersion(string client_name, vector<string> protocol_min_max) = 0;
    virtual ElectrumMessage get_subscription_event() = 0;
    virtual void do_interrupt() = 0;
    virtual void stop() = 0;
    virtual void subscribe_to_error_events(ElectrumErrorCallback error_callback) = 0;
    virtual void clear_error_events_subscriptions() = 0;
};

class XElectrumInterface {
public:
    virtual ElectrumInterface& client() = 0;
};

#endif

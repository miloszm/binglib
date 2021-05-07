#ifndef ELECTRUM_INTERFACE_HPP
#define ELECTRUM_INTERFACE_HPP

#include <string>
#include "binglib/electrum_model.hpp"
//#include "src/electrumclient/electrum_model.hpp"

using namespace std;

class ElectrumInterface {
public:
    virtual void init(string hostname, string service, string certification_file_path) = 0;
    virtual void scripthashSubscribe(string scripthash) = 0;
    virtual AddressHistory getHistory(string address) = 0;
    virtual vector<AddressHistory> getHistoryBulk(vector<string> addresses) = 0;
    virtual string getTransaction(string txid) = 0;
    virtual vector<string> getTransactionBulk(vector<string> txids) = 0;
    virtual AddressBalance getBalance(string address) = 0;
    virtual string getBlockHeader(int height) = 0;
    virtual vector<Utxo> getUtxos(string scripthash) = 0;
    virtual double estimateFee(int wait_blocks) = 0;
    virtual string broadcastTransaction(string txid) = 0;
    virtual void ping() = 0;
    virtual vector<string> getVersion(string client_name, vector<string> protocol_min_max) = 0;
    virtual ElectrumMessage get_subscription_event() = 0;
    virtual void do_interrupt() = 0;
};

class XElectrumInterface {
public:
    virtual ElectrumInterface& client() = 0;
};

#endif

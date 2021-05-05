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
    virtual string getTransaction(string txid) = 0;
    virtual vector<string> getTransactionBulk(vector<string> txids) = 0;
    virtual AddressBalance getBalance(string address) = 0;
    virtual string getBlockHeader(int height) = 0;
    virtual vector<Utxo> getUtxos(string scripthash) = 0;
    virtual double estimateFee(int wait_blocks) = 0;
    virtual string broadcastTransaction(string txid) = 0;
};

#endif

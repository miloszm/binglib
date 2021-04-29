#ifndef ELECTRUM_API_CLIENT_HPP
#define ELECTRUM_API_CLIENT_HPP

#include <string>
#include "electrum_client.hpp"
#include "binglib/electrum_model.hpp"
//#include "src/electrumclient/electrum_model.hpp"

using namespace std;

class ElectrumApiClient {
public:
    ElectrumApiClient(): client_(new ElectrumClient()) {}
    virtual ~ElectrumApiClient() { if (client_) delete client_; }
    void init(string hostname, string service, string certification_file_path);
    void scripthashSubscribe(string scripthash);
    AddressHistory getHistory(string address);
    string getTransaction(string txid);
    AddressBalance getBalance(string address);
    string getBlockHeader(int height);
    vector<Utxo> getUtxos(string scripthash);
    double estimateFee(int wait_blocks);
    string broadcastTransaction(string txid);
    ElectrumMessage run_receiving_loop(std::atomic<bool>& interrupt_requested){ return client_->run_receiving_loop(interrupt_requested); }
    void shutdown(){ client_->shutdown(); };

    static bool is_scripthash_update(const ElectrumMessage& electrum_message);
private:
    ElectrumClient* client_;
    std::atomic<int> id_counter;
    string hostname_;
    string service_;
    string certification_file_path_;

    void process_exception(exception& e, nlohmann::json response, const string& msg);
};

#endif
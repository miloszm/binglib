#ifndef RONGHUA_CLIENT_HPP
#define RONGHUA_CLIENT_HPP

#include "binglib/ronghua_socket_client.hpp"
//#include "src/ronghuaclient/ronghua_socket_client.hpp"
#include "binglib/electrum_model.hpp"
//#include "src/electrumclient/electrum_model.hpp"
#include <string>

using namespace std;
using boost::asio::ip::tcp;


class RonghuaClient {
public:
    RonghuaClient();
    virtual ~RonghuaClient();
    void init(string hostname, string service, string certificationFilePath);
    nlohmann::json send_request(nlohmann::json json_request, int id);
    void send_request_eat_response(nlohmann::json json_request, int id);


    void scripthashSubscribe(string scripthash);
    AddressHistory getHistory(string address);
    string getTransaction(string txid);
    AddressBalance getBalance(string address);
    string getBlockHeader(int height);
    vector<Utxo> getUtxos(string scripthash);
    double estimateFee(int wait_blocks);
    string broadcastTransaction(string txid);
    ElectrumMessage run_receiving_loop(std::atomic<bool>& interrupt_requested){ return client_->run_receiving_loop(interrupt_requested); }

    static bool is_scripthash_update(const ElectrumMessage& electrum_message);


private:
    RonghuaSocketClient* client_;
    boost::asio::io_context* io_context_;
    boost::asio::ssl::context* ctx_;
    tcp::resolver::results_type endpoints_;
    std::mutex mutex_;
    std::atomic<int> id_counter;
    string hostname_;
    string service_;
    string certification_file_path_;

    void process_exception(exception& e, nlohmann::json response, const string& msg);

};

#endif

#ifndef RONGHUA_CLIENT_HPP
#define RONGHUA_CLIENT_HPP

#include <binglib/ronghua_socket_client.hpp>
//#include "src/ronghuaclient/ronghua_socket_client.hpp"
#include <binglib/electrum_model.hpp>
//#include "src/electrumclient/electrum_model.hpp"
#include <string>
#include <binglib/electrum_interface.hpp>
//#include "src/electrumclient/electrum_interface.hpp"

using namespace std;
using boost::asio::ip::tcp;


class RonghuaClient : public ElectrumInterface {
public:
    RonghuaClient();
    virtual ~RonghuaClient();
    void init(string hostname, string service, string certificationFilePath) override;
    nlohmann::json send_request(nlohmann::json json_request, int id);
    void send_request_eat_response(nlohmann::json json_request, int id);


    void scripthashSubscribe(string scripthash) override;
    AddressHistory getHistory(string address) override;
    string getTransaction(string txid) override;
    AddressBalance getBalance(string address) override;
    string getBlockHeader(int height) override;
    vector<Utxo> getUtxos(string scripthash) override;
    double estimateFee(int wait_blocks) override;
    string broadcastTransaction(string txid) override;
    ElectrumMessage get_subscription_event(){ return client_->get_subscription_event(); }
    void do_interrupt();
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
    std::atomic<bool> interrupt_requested_;

    void process_exception(exception& e, nlohmann::json response, const string& msg);

};

#endif

#ifndef ELECTRUM_CLIENT_HPP
#define ELECTRUM_CLIENT_HPP

#include "json_socket_client.hpp"
#include <string>

using namespace std;
using boost::asio::ip::tcp;

struct ElectrumRequest {
    string method;
    int id;
    vector<string> params;
};

void electrum_request_to_json(nlohmann::json& j, const ElectrumRequest& r);


class ElectrumClient {
public:
    ElectrumClient();
    virtual ~ElectrumClient();
    void init(string hostname, string service, string certificationFilePath);
    nlohmann::json send_request(nlohmann::json json_request, int id);
    void send_request_eat_response(nlohmann::json json_request, int id);
    ElectrumMessage run_receiving_loop(std::atomic<bool>& interrupt_requested){ return client_->run_receiving_loop(interrupt_requested); }
    void shutdown(){ client_->shutdown(); };

private:
    JsonSocketClient* client_;
    boost::asio::io_context* io_context_;
    boost::asio::ssl::context* ctx_;
    tcp::resolver::results_type endpoints_;
    std::mutex mutex_;
};

#endif

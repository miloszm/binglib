#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;
using json = nlohmann::json;
using namespace std;
#include "electrum_client.hpp"

void electrum_request_to_json(nlohmann::json &j, const ElectrumRequest &r) {
  j = nlohmann::json{{"jsonrpc", "2.0"},
                     {"method", r.method},
                     {"id", r.id},
                     {"params", r.params}};
}

ElectrumClient::ElectrumClient() : client_(nullptr), io_context_(new boost::asio::io_context()), ctx_(new boost::asio::ssl::context(boost::asio::ssl::context::sslv23)) {}

ElectrumClient::~ElectrumClient() {
    if (client_) delete client_;
    delete io_context_;
    delete ctx_;
}

void ElectrumClient::init(string hostname, string service,
                          string cert_file_path) {
  tcp::resolver resolver(*io_context_);
  endpoints_ = resolver.resolve(hostname, service);

  ctx_->load_verify_file(cert_file_path);

  client_ = new JsonSocketClient(*io_context_, *ctx_, endpoints_);
  io_context_->run();
  client_->prepare_connection.lock();
}

json ElectrumClient::send_request(json json_request, int id) {
  unique_lock<mutex> lock(mutex_);
  client_->send_request(json_request);
  return client_->receive_response(id);
}

void ElectrumClient::send_request_eat_response(json json_request, int id) {
  client_->send_request(json_request);
  client_->eat_response(id);
}

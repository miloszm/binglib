#ifndef JSON_SOCKET_CLIENT_HPP
#define JSON_SOCKET_CLIENT_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include "electrum_input_queue.hpp"

class JsonSocketClient {
public:
  JsonSocketClient(
      boost::asio::io_context &io_context, boost::asio::ssl::context &context,
      const boost::asio::ip::tcp::resolver::results_type &endpoints);
  void send_request(nlohmann::json json_request);
  nlohmann::json receive_response(int id);
  void eat_response(int id);
  ElectrumMessage run_receiving_loop(std::atomic<bool>& interrupt_requested);
  std::mutex prepare_connection;

private:
  bool verify_certificate(bool preverified,
                          boost::asio::ssl::verify_context &ctx);
  void connect(const boost::asio::ip::tcp::resolver::results_type &endpoints);
  void handshake();

  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
  ElectrumInputQueue queue_;
  std::atomic<bool> is_loop;

public:
  static ElectrumMessage from_json(nlohmann::json message);
};

#endif

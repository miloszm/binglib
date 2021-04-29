#ifndef RONGHUA_SOCKET_CLIENT_HPP
#define RONGHUA_SOCKET_CLIENT_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include "binglib/ronghua_input_queue.hpp"
//#include "src/ronghuaclient/ronghua_input_queue.hpp"
#include "binglib/electrum_model.hpp"
//#include "src/electrumclient/electrum_model.hpp"

class RonghuaSocketClient {
public:
    RonghuaSocketClient(
            boost::asio::io_context &io_context, boost::asio::ssl::context &context,
            const boost::asio::ip::tcp::resolver::results_type &endpoints);
    void send_request(nlohmann::json json_request);
    nlohmann::json receive_response(int id);
    void eat_response(int id);
    void run_receiving_loop(std::atomic<bool>& interrupt_requested);
    ElectrumMessage get_subscription_event();
    std::mutex prepare_connection;

private:
    bool verify_certificate(bool preverified,
                            boost::asio::ssl::verify_context &ctx);
    void connect(const boost::asio::ip::tcp::resolver::results_type &endpoints);
    void handshake();
    void do_read(const boost::system::error_code& error, size_t length, std::ostringstream& oss, boost::array<char, 512>& buf);

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    RonghuaInputQueue queue_;
    std::mutex read_mutex_;

public:
    static ElectrumMessage from_json(nlohmann::json message);
};

#endif

#ifndef RONGHUA_SOCKET_CLIENT_HPP
#define RONGHUA_SOCKET_CLIENT_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include "binglib/ronghua_input_queue.hpp"
//#include "src/ronghuaclient/ronghua_input_queue.hpp"
#include "binglib/electrum_interface.hpp"
//#include "src/electrumclient/electrum_interface.hpp"

class RonghuaSocketClient {
public:
    RonghuaSocketClient(
            boost::asio::io_context &io_context, boost::asio::ssl::context &context,
            const boost::asio::ip::tcp::resolver::results_type &endpoints,
            std::atomic<bool>& interrupt_requested,
            vector<ElectrumErrorCallback>& electrum_error_callbacks);
    virtual ~RonghuaSocketClient();
    void send_request(nlohmann::json json_request);
    nlohmann::json receive_response(int id);
    vector<nlohmann::json> receive_response_bulk(vector<int> ids);
    void eat_response(int id);
    void run_receiving_loop();
    ElectrumMessage get_subscription_event();
    void do_interrupt();
    void stop();
//    std::mutex prepare_connection;

private:
    bool verify_certificate(bool preverified,
                            boost::asio::ssl::verify_context &ctx);
    void connect(const boost::asio::ip::tcp::resolver::results_type &endpoints);
    void handshake();
    void do_read(const boost::system::error_code& error, size_t length);
    void async_read();
    void push_error(int error_code, std::string error_message);
//    void timeout_handler(const boost::system::error_code& e);

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    boost::asio::io_context* io_context_;
    RonghuaInputQueue queue_;
    std::mutex read_mutex_;
    std::atomic<bool>& interrupt_requested_;
    boost::array<char, 512> buf;
    std::ostringstream oss;
    vector<ElectrumErrorCallback>& electrum_error_callbacks_;
//    std::unique_ptr<boost::asio::deadline_timer> timer_;

public:
    static ElectrumMessage from_json(nlohmann::json message);
};

#endif


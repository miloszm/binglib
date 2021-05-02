#include "ronghua_socket_client.hpp"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread.hpp>
#include <cstring>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;
using json = nlohmann::json;
using namespace std;
using namespace std::chrono_literals;

RonghuaSocketClient::RonghuaSocketClient(boost::asio::io_context &io_context,
                                   boost::asio::ssl::context &context,
                                   const tcp::resolver::results_type &endpoints,
                                   std::atomic<bool>& interrupt_requested)
        : socket_(io_context, context), interrupt_requested_(interrupt_requested) {
    prepare_connection.lock();
    socket_.set_verify_mode(boost::asio::ssl::verify_none);
    socket_.set_verify_callback(
            std::bind(&RonghuaSocketClient::verify_certificate, this, std::placeholders::_1, std::placeholders::_2));
    connect(endpoints);
}

RonghuaSocketClient::~RonghuaSocketClient() {
    oss.str("");
    oss.clear();
}

bool RonghuaSocketClient::verify_certificate(
        bool preverified, boost::asio::ssl::verify_context &ctx) {
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // In this example we will simply print the certificate's subject name.
    char subject_name[256];
    X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);

    return preverified;
}

void RonghuaSocketClient::connect(const tcp::resolver::results_type &endpoints) {
    boost::asio::async_connect(socket_.lowest_layer(), endpoints,
                               [this](const boost::system::error_code &error,
                                      const tcp::endpoint & /*endpoint*/) {
                                   if (!error) {
                                       handshake();
                                   } else {
                                       throw std::runtime_error(error.message());
                                   }
                               });
}

void RonghuaSocketClient::handshake() {
    socket_.async_handshake(boost::asio::ssl::stream_base::client,
                            [this](const boost::system::error_code &error) {
                                if (!error) {
                                    prepare_connection.unlock();
                                } else {
                                    throw std::runtime_error(error.message());
                                }
                            });
}

void RonghuaSocketClient::send_request(json json_request) {
    enum { max_length = 1536 };
    char request_[max_length];
    std::string req0 = json_request.dump();
    std::string req = req0 + "\n";
    strcpy(request_, req.data());
    size_t request_length = strlen(req.data());
    //cout << "sending request: " << req << "\n";

    boost::asio::write(socket_, boost::asio::buffer(request_, request_length));
}

json RonghuaSocketClient::receive_response(int id) {
    ElectrumMessage reply_message = queue_.pop_reply(id);
    return reply_message.message;
}

void RonghuaSocketClient::eat_response(int id) {
    queue_.pop_eat_reply(id);
}

void RonghuaSocketClient::run_receiving_loop(boost::asio::io_context* io_context) {
    io_context_ = io_context;
    async_read();
    if (io_context_->stopped()){
        io_context_->restart();
        boost::thread t(boost::bind(&boost::asio::io_context::run, io_context_));
    }
}

void RonghuaSocketClient::async_read() {
    if (!interrupt_requested_) {
        socket_.async_read_some(boost::asio::buffer(buf, 512),
                                boost::bind(&RonghuaSocketClient::do_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
}

void RonghuaSocketClient::do_read(const boost::system::error_code& error, size_t length) {
    //unique_lock<mutex> lock(read_mutex_);

    if (error == boost::asio::error::eof) {
        throw std::invalid_argument(
                string("socket eof error: ") + to_string(error.value()) + " (" + error.message() + ")");
    }
    else if (error.value() == 60 || error.value() == 1) {
        // not throwing exception in case of timeout
        cout << string("socket reading error: ") + to_string(error.value()) + " (" + error.message() + ")";
        std::this_thread::sleep_for(5000ms);
    }
    else if (error) {
        throw std::invalid_argument(
                string("socket reading error: ") + to_string(error.value()) + " (" + error.message() + ")");
    }

    if (length > 0) {
        oss.write(buf.data(), length);
        std::string candidate_response = oss.str();
        try {
            json parsed_response = json::parse(candidate_response);
            ElectrumMessage message = from_json(parsed_response);
            queue_.push(message);
            oss.str("");
            oss.clear();
        } catch (json::parse_error &e) {
            // not yet parsable, keep reading
        }
    }
    async_read();
}

ElectrumMessage RonghuaSocketClient::get_subscription_event() {
    return queue_.pop_reply(-1);
}

ElectrumMessage RonghuaSocketClient::from_json(nlohmann::json message) {
    string method;
    int id {0};
    bool has_correlaton_id;
    vector<string> params;
    try { message.at("method").get_to(method); } catch (exception& e){}
    try {
        message.at("id").get_to(id);
        has_correlaton_id = true;
    } catch (exception& e){
        has_correlaton_id = false;
    }
    try { message.at("params").get_to(params); } catch (exception& e){}
    return ElectrumMessage{
            message,
            method,
            has_correlaton_id,
            id,
            params
    };
}

void RonghuaSocketClient::do_interrupt() {
    cout << "entering RonghuaSocketClient::do_interrupt\n";
    ElectrumMessage poison_message{json::parse("{}"), "", false, 0};
    queue_.push(poison_message);
    io_context_->stop();
}

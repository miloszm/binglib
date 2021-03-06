/**
 * Copyright (c) 2020-2021 binglib developers (see AUTHORS)
 *
 * This file is part of binglib.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
                                   std::atomic<bool>& interrupt_requested,
                                   vector<ElectrumErrorCallback>& electrum_error_callbacks
                                   )
        : socket_(io_context, context), io_context_(&io_context), interrupt_requested_(interrupt_requested), electrum_error_callbacks_(electrum_error_callbacks) {
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
                                       push_error(error.value(), error.message());
                                   }
                               });
}

void RonghuaSocketClient::handshake() {
    socket_.async_handshake(boost::asio::ssl::stream_base::client,
                            [this](const boost::system::error_code &error) {
                                if (error) {
                                    push_error(error.value(), error.message());
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
    boost::asio::write(socket_, boost::asio::buffer(request_, request_length));
}

json RonghuaSocketClient::receive_response(int id) {
    ElectrumMessage reply_message = queue_.pop_reply(id);
    return reply_message.message;
}

vector<json> RonghuaSocketClient::receive_response_bulk(vector<int> ids) {
    vector<ElectrumMessage> reply_messages = queue_.pop_reply_bulk(ids);
    vector<json> r;
    for (auto reply_message: reply_messages){
        r.push_back(reply_message.message);
    }
    return r;
}

void RonghuaSocketClient::eat_response(int id) {
    queue_.pop_eat_reply(id);
}

void RonghuaSocketClient::run_receiving_loop() {
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
    if (error) {
        push_error(error.value(), error.message());
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
    ElectrumMessage poison_message{json::parse("{}"), "", false, 0};
    queue_.push(poison_message);
    io_context_->stop();
}

void RonghuaSocketClient::push_error(int error_code, std::string error_message) {
    ElectrumErrorEvent event{error_code, error_message};
    for (auto f: electrum_error_callbacks_) {
        f(event);
    }
}

void RonghuaSocketClient::stop() {
    socket_.lowest_layer().close();
}

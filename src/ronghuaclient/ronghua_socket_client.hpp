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

private:
    bool verify_certificate(bool preverified,
                            boost::asio::ssl::verify_context &ctx);
    void connect(const boost::asio::ip::tcp::resolver::results_type &endpoints);
    void handshake();
    void do_read(const boost::system::error_code& error, size_t length);
    void async_read();
    void push_error(int error_code, std::string error_message);

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    boost::asio::io_context* io_context_;
    RonghuaInputQueue queue_;
    std::mutex read_mutex_;
    std::atomic<bool>& interrupt_requested_;
    boost::array<char, 512> buf;
    std::ostringstream oss;
    vector<ElectrumErrorCallback>& electrum_error_callbacks_;

public:
    static ElectrumMessage from_json(nlohmann::json message);
};

#endif


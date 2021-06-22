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
#include "libb_client.hpp"
//#include <binglib/bing_common.hpp>
#include "src/common/bing_common.hpp"

// TODO
// eliminate all cout here and convert to exceptions

void LibbClient::init(std::string url) {
  url_ = url;
  re_init();
}

void LibbClient::re_init() {
  if (client_) {
    delete client_;
  }
  client_ = new client::obelisk_client(connection_);
  connection_.server = config::endpoint(url_);
  do_connect(*client_);
}

void LibbClient::do_connect(client::obelisk_client &client) {
  if (!client.connect(connection_)) {
    throw std::invalid_argument("Connection Failed");
  }
}

size_t LibbClient::fetch_height() {
  size_t height;

  const auto on_error = [](const code &ec) {
    throw std::invalid_argument("Error Code: " + ec.message());
  };

  auto on_reply = [&height](size_t blockHeight) { height = blockHeight; };

  client_->blockchain_fetch_last_height(on_error, on_reply);
  client_->wait();
  return height;
}

void LibbClient::fetch_utxo(const wallet::payment_address address,
                            uint64_t satoshis,
                            wallet::select_outputs::algorithm algorithm,
                            chain::points_value &points_value) {

  const auto on_error = [](const code &ec) {
    throw std::invalid_argument("Error Code: " + ec.message());
  };

  auto on_reply = [&points_value](const chain::points_value &pv) {
    points_value = pv;
  };

  client_->blockchain_fetch_unspent_outputs(on_error, on_reply, address,
                                            satoshis, algorithm);
  client_->wait();

  cout << "LibbClient::fetch_utxo " << address << "\n";
}

void LibbClient::fetch_history(const wallet::payment_address &address,
                               vector<chain::history> &history) {
  std::ostringstream oss;
  bool is_error = false;

  const auto on_error = [&](const code &ec) {
    oss << "Fetch history failed with error code: " << ec.message();
    is_error = true;
  };

  auto on_reply = [&](const chain::history::list &history_list) {
    for (auto h : history_list) {
      history.push_back(h);
    }
  };

  client_->blockchain_fetch_history3(on_error, on_reply, address);
  client_->wait();
  if (is_error)
    throw std::invalid_argument(oss.str());
}

void LibbClient::fetch_tx(std::string tx_id, chain::transaction &transaction) {
  std::ostringstream oss;
  bool is_error = false;
  chain::transaction tr;

  const auto on_error = [&](const code &ec) {
    oss << "Fetch transaction failed with error code: " << ec.message();
    is_error = true;
  };

  auto on_reply = [&](const chain::transaction &transaction_) {
    transaction = transaction_;
  };

  hash_digest hash;
  decode_hash(hash, tx_id);
  client_->blockchain_fetch_transaction2(on_error, on_reply, hash);
  client_->wait();
  if (is_error)
    throw std::invalid_argument(oss.str());
}

void LibbClient::fetch_header(int height, chain::header &header) {
  std::ostringstream oss;
  bool is_error = false;
  chain::transaction tr;

  const auto on_error = [&](const code &ec) {
    oss << "Fetch header failed with error code: " << ec.message();
    is_error = true;
  };

  auto on_reply = [&](const chain::header &header_) { header = header_; };

  client_->blockchain_fetch_block_header(on_error, on_reply, height);
  client_->wait();
  if (is_error)
    throw std::invalid_argument(oss.str());

  // cout << "LibbClient::fetch_header " << height << "\n";
}

void LibbClient::send_tx(std::string tx_hex) {
  std::ostringstream oss;
  bool is_error = false;
  const auto on_error = [&](const code &ec) {
    oss << "Send failed with error code: " << ec.message();
    is_error = true;
  };

  const auto on_reply = [&](const code &ec) {
    if (ec != error::success) {
      oss << "Send failed " << ec.message();
      is_error = true;
    }
  };

  chain::transaction tx;
  data_chunk tx_chunk;
  if (!decode_base16(tx_chunk, tx_hex)) {
    throw std::invalid_argument("could not decode transaction from hex");
  }

  if (!tx.from_data(tx_chunk)) {
    throw std::invalid_argument("could not decode transaction");
  } else {
    client_->transaction_pool_broadcast(on_error, on_reply, tx);
    client_->wait();
    if (is_error)
      throw std::invalid_argument(oss.str());
  }
}

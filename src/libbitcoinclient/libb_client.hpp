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
#ifndef BING_CLIENT_HPP
#define BING_CLIENT_HPP

#include <bitcoin/system.hpp>
#include <bitcoin/client.hpp>
#include <vector>

using namespace bc;
using namespace std;

class LibbClient {
public:
  LibbClient()
      : connection_{3,8}, client_(nullptr) {}
  virtual ~LibbClient(){ if (client_) delete client_; }
  void init(std::string url);
  void re_init();
  size_t fetch_height();
  void fetch_utxo(const wallet::payment_address address, uint64_t satoshis,
                  wallet::select_outputs::algorithm,
                  chain::points_value &points_value);
  void fetch_history(const wallet::payment_address& address, vector<chain::history>& history);
  void fetch_tx(std::string tx_id, chain::transaction& transaction);
  void fetch_header(int height, chain::header& header);
  void send_tx(std::string tx_hex);

private:
  client::connection_type connection_;
  client::obelisk_client* client_;
  std::string url_;
  void do_connect(client::obelisk_client &client);
};

#endif
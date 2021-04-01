#ifndef BING_CLIENT_HPP
#define BING_CLIENT_HPP

#include "src/common/bing_common.hpp"
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>

using namespace bc;

class LibbClient {
public:
  LibbClient()
      : connection{3,8},
        client{connection} {}
  void init(std::string url);
  size_t fetch_height();
  void fetch_utxo(const wallet::payment_address address, uint64_t satoshis,
                  wallet::select_outputs::algorithm,
                  chain::points_value &points_value);

private:
  client::connection_type connection;
  client::obelisk_client client;
  void do_connect(client::obelisk_client &client);
};

#endif
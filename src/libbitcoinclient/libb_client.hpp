#ifndef BING_CLIENT_HPP
#define BING_CLIENT_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <vector>

using namespace bc;
using namespace std;

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
  void fetch_history(const wallet::payment_address& address, vector<chain::history>& history);
  void fetch_tx(std::string tx_id, chain::transaction& transaction);
  void send_tx(std::string tx_hex);

private:
  client::connection_type connection;
  client::obelisk_client client;
  void do_connect(client::obelisk_client &client);
};

#endif
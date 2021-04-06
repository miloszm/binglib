#include <binglib/bing_common.hpp>
#include "libb_client.hpp"

// TODO
// eliminate all cout here and convert to exceptions

void LibbClient::init(std::string url) {
  connection.server = config::endpoint(url);
  do_connect(client);
}

void LibbClient::do_connect(client::obelisk_client &client) {
  if (!client.connect(connection)) {
    std::cout << "Connection Failed" << std::endl;
  }
}

size_t LibbClient::fetch_height() {
  size_t height;

  const auto on_error = [](const code &ec) {
    std::cout << "Error Code: " << ec.message() << std::endl;
  };

  auto on_reply = [&height](size_t blockHeight) { height = blockHeight; };

  client.blockchain_fetch_last_height(on_error, on_reply);
  client.wait();
  return height;
}

void LibbClient::fetch_utxo(const wallet::payment_address address,
                            uint64_t satoshis,
                            wallet::select_outputs::algorithm algorithm,
                            chain::points_value &points_value) {

  const auto on_error = [](const code &ec) {
    std::cout << "Error Code: " << ec.message() << std::endl;
  };

  auto on_reply = [&points_value](const chain::points_value &pv) {
    points_value = pv;
  };

  client.blockchain_fetch_unspent_outputs(on_error, on_reply, address, satoshis,
                                          algorithm);
  client.wait();
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
    client.transaction_pool_broadcast(on_error, on_reply, tx);
    client.wait();
    if (is_error)
        throw std::invalid_argument(oss.str());
  }
}

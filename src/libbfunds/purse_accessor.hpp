#ifndef PURSE_ACCESSOR_HPP
#define PURSE_ACCESSOR_HPP

//#include <binglib/bing_common.hpp>
#include "src/common/bing_common.hpp"
//#include <binglib/electrum_interface.hpp>
#include "src/electrumclient/electrum_interface.hpp"
//#include <binglib/libb_client.hpp>
#include "src/libbitcoinclient/libb_client.hpp"
#include <bitcoin/system.hpp>

using namespace std;

struct AddressFundsInfo {
    string address;
    uint64_t requested_funds;
    uint64_t actual_funds;
};

struct AddressFunds {
  string address;
  uint64_t requested_funds;
  uint64_t actual_funds;
  std::vector<libbitcoin::chain::output_point> points;
};

struct UtxoInfo {
    string address;
    string tx;
    uint32_t output_index;
    uint64_t value;
    string script;
};

struct HistoryItem {
    string address;
    string txid;
    string txhex;
    int height;
};

class PurseAccessor {
public:
  static AddressFunds obtain_funds(LibbClient &libb_client,
                                   uint64_t requested_funds,
                                   string address);
  static AddressFunds look_for_funds(LibbClient &libb_client,
                                     uint64_t requested_funds,
                                     std::vector<string> &addresses);
  static AddressFunds
  look_for_funds_by_balance(ElectrumInterface &electrum_client,
                            LibbClient &libb_client, uint64_t requested_funds,
                            std::vector<string> &addresses,
                            map<string, uint64_t> &address_to_balance);
  static void scan_balances(ElectrumInterface &electrum_client,
                            std::vector<string> &addresses,
                            map<string, uint64_t> &address_to_balance);
  static AddressFundsInfo
  look_for_address_with_balance(uint64_t requested_funds,
                                std::vector<string> &addresses,
                                map<string, uint64_t> &address_to_balance);

  static void find_utxos(LibbClient &libb_client, vector<string>& addresses, map<string, uint64_t>& address_to_balance, map<string, vector<UtxoInfo>>& address_to_utxos);

  static void find_history(ElectrumInterface &electrum_api_client, LibbClient &libb_client, vector<string>& addresses, vector<HistoryItem>& history_items);
};

#endif

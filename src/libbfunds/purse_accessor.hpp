#ifndef PURSE_ACCESSOR_HPP
#define PURSE_ACCESSOR_HPP

#include <binglib/bing_common.hpp>
#include <binglib/electrum_api_client.hpp>
#include <binglib/libb_client.hpp>
#include <bitcoin/bitcoin.hpp>

struct AddressFundsInfo {
    std::string address;
    uint64_t requested_funds;
    uint64_t actual_funds;
};

struct AddressFunds {
  std::string address;
  uint64_t requested_funds;
  uint64_t actual_funds;
  std::vector<libbitcoin::chain::output_point> points;
};

class PurseAccessor {
public:
  static AddressFunds obtain_funds(LibbClient &libb_client,
                                   uint64_t requested_funds,
                                   std::string address);
  static AddressFunds look_for_funds(LibbClient &libb_client,
                                     uint64_t requested_funds,
                                     std::vector<std::string> &addresses);
  static AddressFunds
  look_for_funds_by_balance(ElectrumApiClient &electrum_client,
                            LibbClient &libb_client, uint64_t requested_funds,
                            std::vector<std::string> &addresses,
                            map<string, uint64_t> &address_to_balance);
  static void scan_balances(ElectrumApiClient &electrum_client,
                            std::vector<std::string> &addresses,
                            map<string, uint64_t> &address_to_balance);
  static AddressFundsInfo
  look_for_address_with_balance(uint64_t requested_funds,
                                std::vector<std::string> &addresses,
                                map<string, uint64_t> &address_to_balance);
};

#endif

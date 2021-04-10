#include "purse_accessor.hpp"
#include "funds_finder.hpp"
#include <binglib/address_converter.hpp>

using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;

AddressFunds PurseAccessor::obtain_funds(LibbClient &libb_client,
                                         uint64_t requested_funds,
                                         string address) {
  chain::points_value points_value;
  libb_client.fetch_utxo(payment_address(address), requested_funds,
                         wallet::select_outputs::algorithm::individual,
                         points_value);
  auto utxos_funds = FundsFinder::find_funds(requested_funds, points_value);
  return AddressFunds{address, requested_funds, utxos_funds.second,
                      utxos_funds.first};
}

AddressFunds PurseAccessor::look_for_funds(LibbClient &libb_client,
                                           uint64_t requested_funds,
                                           vector<string> &addresses) {
  AddressFunds maxFunds = AddressFunds{"", 0, 0, vector<output_point>()};
  for (auto a : addresses) {
    auto funds = obtain_funds(libb_client, requested_funds, a);
    if (funds.actual_funds >= requested_funds)
      return funds;
    if (funds.actual_funds >= maxFunds.actual_funds)
      maxFunds = funds;
  }
  return maxFunds;
}

AddressFunds PurseAccessor::look_for_funds_by_balance(
    ElectrumApiClient &electrum_api_client, LibbClient &libb_client,
    uint64_t requested_funds, vector<string> &addresses,
    map<string, uint64_t> &address_to_balance) {
  AddressFunds maxFunds = AddressFunds{"", 0, 0, vector<output_point>()};
  for (auto a : addresses) {
    string address_spkh = AddressConverter::base58_to_spkh_hex(a);
    AddressBalance balance = electrum_api_client.getBalance(address_spkh);
    address_to_balance[a] = balance.confirmed;
    if (balance.confirmed >= requested_funds) {
      auto funds = obtain_funds(libb_client, requested_funds, a);
      if (funds.actual_funds >= requested_funds)
        return funds;
      if (funds.actual_funds >= maxFunds.actual_funds)
        maxFunds = funds;
    } else {
      if (balance.confirmed >= maxFunds.actual_funds) {
        uint64_t balance_actual = balance.confirmed;
        maxFunds = AddressFunds{a, requested_funds, balance_actual,
                                vector<output_point>()};
      }
    }
  }
  return maxFunds;
}

void PurseAccessor::scan_balances(ElectrumApiClient &electrum_api_client,
                                  vector<string> &addresses,
                                  map<string, uint64_t> &address_to_balance) {
  for (auto a : addresses) {
    string address_spkh = AddressConverter::base58_to_spkh_hex(a);
    AddressBalance balance = electrum_api_client.getBalance(address_spkh);
    address_to_balance[a] = balance.confirmed;
  }
}

AddressFundsInfo PurseAccessor::look_for_address_with_balance(
    uint64_t requested_funds,
    vector<string> &addresses, map<string, uint64_t> &address_to_balance) {
  AddressFundsInfo maxFunds = AddressFundsInfo{"", 0, 0};
  for (string& a : addresses) {
    uint64_t balance = address_to_balance[a];
    if (balance >= requested_funds) {
        return AddressFundsInfo{a, requested_funds, balance};
    } else {
      if (balance >= maxFunds.actual_funds) {
        maxFunds = AddressFundsInfo{a, requested_funds, balance};
      }
    }
  }
  return maxFunds;
}

void PurseAccessor::find_utxos(LibbClient &libb_client, string address, vector<UtxoInfo>& utxos){
    chain::points_value points_value;
    libb_client.fetch_utxo(payment_address(address), 1,
                           wallet::select_outputs::algorithm::individual,
                           points_value);
    for (auto p = begin(points_value.points);
         p != end(points_value.points); ++p) {
        UtxoInfo utxo_info {
            address,
            encode_hash(p->hash()),
            p->index(),
            p->value()
        };
        utxos.push_back(utxo_info);
    }
}

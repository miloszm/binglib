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
#include "purse_accessor.hpp"
#include "funds_finder.hpp"
//#include <binglib/address_converter.hpp>
#include "src/utility/address_converter.hpp"

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
        ElectrumInterface &electrum_api_client, LibbClient &libb_client,
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

void PurseAccessor::scan_balances(ElectrumInterface &electrum_api_client,
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
    const vector<string> &addresses, const map<string, uint64_t> &address_to_balance) {
  AddressFundsInfo maxFunds = AddressFundsInfo{"", 0, 0};
  for (const string& a : addresses) {
    uint64_t balance = address_to_balance.at(a);
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

void PurseAccessor::find_utxos(LibbClient &libb_client, vector<string>& addresses, map<string, uint64_t>& address_to_balance, map<string, vector<UtxoInfo>>& address_to_utxos){
    for (string& address: addresses) {
        if (address_to_balance[address] > 0) {
            if (address_to_utxos[address].empty()) {
                vector<UtxoInfo> utxos;
                chain::points_value points_value;
                libb_client.fetch_utxo(payment_address(address), 1,
                                       wallet::select_outputs::algorithm::individual,
                                       points_value);
                for (auto p = begin(points_value.points);
                     p != end(points_value.points); ++p) {
                    UtxoInfo utxo_info{
                            address,
                            encode_hash(p->hash()),
                            p->index(),
                            p->value()
                    };
                    utxos.push_back(utxo_info);
                }
                address_to_utxos[address] = utxos;
            }
        }
    }
}


//void PurseAccessor::find_history(ElectrumApiClient &electrum_api_client,LibbClient &libb_client, vector<string>& addresses, vector<HistoryItem>& history_items){
//    for (const string& address: addresses) {
//        string address_spkh = AddressConverter::base58_to_spkh_hex(address);
//        AddressHistory history = electrum_api_client.getHistory(address_spkh);
//        for (const AddressHistoryItem& history_item: history){
//            chain::transaction tx;
//            libb_client.fetch_tx(history_item.txid, tx);
//            for (auto& i: tx.inputs()){
//                HistoryItem item;
//                item.address = address;
//                item.txid = history_item.txid;
//                chain::transaction tr;
//                string funding_tx = encode_hash(i.previous_output().hash());
//                int funding_idx = i.previous_output().index();
//                libb_client.fetch_tx(funding_tx, tr);
//                uint64_t value = 0;
//                if ( funding_idx < tr.outputs().size()){
//                    value = tr.outputs().at(funding_idx).value();
//                }
//                item.input = UtxoInfo{address, funding_tx, static_cast<uint32_t>(funding_idx), value};
//                item.output = OutputInfo{ 0, ""};
//                history_items.push_back(item);
//            }
//            for (auto& o: tx.outputs()){
//                HistoryItem item;
//                item.address = address;
//                item.txid = history_item.txid;
//                item.output = OutputInfo{o.value(), to_string(static_cast<int>(o.script().pattern()))};
//                item.input = UtxoInfo{"", "", 0, 0, ""};
//                history_items.push_back(item);
//            }
//        }
//    }
//}

void PurseAccessor::find_history(ElectrumInterface &electrum_api_client,LibbClient &libb_client, vector<string>& addresses, vector<HistoryItem>& history_items){
    for (const string& address: addresses) {
        string address_spkh = AddressConverter::base58_to_spkh_hex(address);
        AddressHistory history = electrum_api_client.getHistory(address_spkh);
        for (const AddressHistoryItem& history_item: history){
            HistoryItem item;
            item.address = address;
            item.txid = history_item.txid;
            item.height = history_item.height;
            item.txhex = electrum_api_client.getTransaction(history_item.txid);
            history_items.push_back(item);
        }
    }
}

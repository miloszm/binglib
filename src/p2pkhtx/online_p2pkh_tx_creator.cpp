#include <binglib/bing_common.hpp>
#include <bitcoin/system.hpp>
#include <binglib/libb_client.hpp>
//#include <src/libbitcoinclient/libb_client.hpp>
#include <binglib/funds_finder.hpp>
//#include "src/libbfunds/funds_finder.hpp"
#include <binglib/address_converter.hpp>
#include "online_p2pkh_tx_creator.hpp"


using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;


string OnlineP2pkhTxCreator::construct_p2pkh_tx_from_address(
        LibbClient &libb_client,
        const string src_addr,
        const string priv_key_wif,
        const uint64_t amount_to_transfer,
        const uint64_t satoshis_fee,
        const std::string target_addr
){
    const wallet::ec_private priv_key_ec(priv_key_wif);
    return construct_p2pkh_tx_from_address(libb_client, src_addr, priv_key_ec, amount_to_transfer, satoshis_fee, target_addr);
}

string OnlineP2pkhTxCreator::construct_p2pkh_tx_from_address(
        LibbClient &libb_client,
        const string src_addr,
        const ec_private priv_key_ec,
        const uint64_t amount_to_transfer,
        const uint64_t satoshis_fee,
        const std::string target_addr
) {
    chain::points_value points_value;
    libb_client.fetch_utxo(payment_address(src_addr), 1, wallet::select_outputs::algorithm::individual, points_value);
    auto satoshis_needed = amount_to_transfer + satoshis_fee;
    auto utxos_funds = FundsFinder::find_funds(satoshis_needed, points_value);
    auto utxos = utxos_funds.first;
    auto available_funds = utxos_funds.second;
    return do_construct_p2pkh_tx_from_address(utxos, available_funds, src_addr, priv_key_ec, amount_to_transfer, satoshis_fee,
                                              target_addr);
}

string OnlineP2pkhTxCreator::construct_p2pkh_tx_from_address(
        ElectrumInterface &electrum_interface,
        const string src_addr,
        const string priv_key_wif,
        const uint64_t amount_to_transfer,
        const uint64_t satoshis_fee,
        const std::string target_addr
){
    const wallet::ec_private priv_key_ec(priv_key_wif);
    return construct_p2pkh_tx_from_address(electrum_interface, src_addr, priv_key_ec, amount_to_transfer, satoshis_fee, target_addr);
}

string OnlineP2pkhTxCreator::construct_p2pkh_tx_from_address(
        ElectrumInterface &electrum_interface,
        const string src_addr,
        const ec_private priv_key_ec,
        const uint64_t amount_to_transfer,
        const uint64_t satoshis_fee,
        const std::string target_addr
) {
    vector<Utxo> input_utxos = electrum_interface.getUtxos(AddressConverter::base58_to_spkh_hex(src_addr));
    auto satoshis_needed = amount_to_transfer + satoshis_fee;
    auto utxos_funds = FundsFinder::find_funds(satoshis_needed, input_utxos);
    auto utxos = utxos_funds.first;
    auto available_funds = utxos_funds.second;
    return do_construct_p2pkh_tx_from_address(utxos, available_funds, src_addr, priv_key_ec, amount_to_transfer, satoshis_fee,
                                              target_addr);
}

string OnlineP2pkhTxCreator::do_construct_p2pkh_tx_from_address(
            const std::vector<libbitcoin::chain::output_point>& utxos,
            uint64_t available_funds,
            const string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const string target_addr
) {
    const wallet::ec_public pub_key = priv_key_ec.to_public();
    const libbitcoin::config::base16 priv_key = libbitcoin::config::base16(priv_key_ec.secret());
    data_chunk pub_key_data_chunk;
    pub_key.to_data(pub_key_data_chunk);
    auto satoshis_needed = amount_to_transfer + satoshis_fee;
    if (utxos.empty()){
        ostringstream oss;
        oss << "Insufficient funds, required " << satoshis_needed << ", maximum available " << available_funds;
        throw std::invalid_argument(oss.str());
    }
    auto refund = available_funds - satoshis_needed;

    // output 0
    script current_locking_script =
            script().to_pay_key_hash_pattern(payment_address(target_addr).hash());
    output output0(amount_to_transfer, current_locking_script);

    // tx
    transaction tx = transaction();
    for(auto utxo: utxos){
        input input1 = input();
        input1.set_previous_output(utxo);
        input1.set_sequence(0xfffffffe);
        tx.inputs().push_back(input1);
    }
    tx.outputs().push_back(output0);
    if (refund > 0){
        output output1(refund, script().to_pay_key_hash_pattern(payment_address(src_addr).hash()));
        tx.outputs().push_back(output1);
    }
    tx.set_version(1);

    // set unlocking script in inputs
    for (unsigned int i = 0; i < utxos.size(); ++i) {
        // sig
        script previous_locking_script = script().to_pay_key_hash_pattern(bitcoin_short_hash(pub_key_data_chunk));
        endorsement sig;
        previous_locking_script.create_endorsement(sig, priv_key_ec.secret(), previous_locking_script, tx, i, all);
        // unlocking previous
        operation::list sigScript;
        sigScript.push_back(operation(sig));
        sigScript.push_back(operation(pub_key_data_chunk));
        script script_unlocking_previous_locking_script(sigScript);
        tx.inputs()[i].set_script(script_unlocking_previous_locking_script);
    }

    return encode_base16(tx.to_data());
}

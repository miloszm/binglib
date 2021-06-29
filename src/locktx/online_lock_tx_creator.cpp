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
#include "online_lock_tx_creator.hpp"
#include "src/libbscript/redeem_script.hpp"
//#include <binglib/address_converter.hpp>
#include "src/utility/address_converter.hpp"
//#include <binglib/funds_finder.hpp>
#include <src/libbfunds/funds_finder.hpp>


using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;


LockTxInfo OnlineLockTxCreator::construct_p2sh_time_locking_tx_from_address(
        LibbClient &libb_client,
        const string src_addr,
        const string priv_key_wif,
        const uint64_t amount_to_transfer,
        const uint64_t satoshis_fee,
        const uint32_t lock_until
){
    const wallet::ec_private priv_key_ec(priv_key_wif);
    return construct_p2sh_time_locking_tx_from_address(libb_client, src_addr, priv_key_ec, amount_to_transfer, satoshis_fee, lock_until);
}

LockTxInfo OnlineLockTxCreator::construct_p2sh_time_locking_tx_from_address(
        LibbClient &libb_client,
        const string src_addr,
        const ec_private priv_key_ec,
        const uint64_t amount_to_transfer,
        const uint64_t satoshis_fee,
        const uint32_t lock_until
) {
    chain::points_value points_value;
    payment_address source_payment_address(src_addr);
    if (!source_payment_address) {
        throw InvalidAddressException("Invalid source address");
    }
    libb_client.fetch_utxo(source_payment_address, 1, wallet::select_outputs::algorithm::individual, points_value);
    auto satoshis_needed = amount_to_transfer + satoshis_fee;
    auto utxos_funds = FundsFinder::find_funds(satoshis_needed, points_value);
    auto utxos = utxos_funds.first;
    auto available_funds = utxos_funds.second;
    return do_construct_p2sh_time_locking_tx_from_address(utxos, available_funds, src_addr, priv_key_ec,
                                                          amount_to_transfer, satoshis_fee, lock_until);
}

LockTxInfo OnlineLockTxCreator::construct_p2sh_time_locking_tx_from_address(
        ElectrumInterface &electrum_interface,
        const string src_addr,
        const string priv_key_wif,
        const uint64_t amount_to_transfer,
        const uint64_t satoshis_fee,
        const uint32_t lock_until
){
    const wallet::ec_private priv_key_ec(priv_key_wif);
    return construct_p2sh_time_locking_tx_from_address(electrum_interface, src_addr, priv_key_ec, amount_to_transfer, satoshis_fee, lock_until);
}

LockTxInfo OnlineLockTxCreator::construct_p2sh_time_locking_tx_from_address(
        ElectrumInterface &electrum_interface,
        const string src_addr,
        const ec_private priv_key_ec,
        const uint64_t amount_to_transfer,
        const uint64_t satoshis_fee,
        const uint32_t lock_until
) {
    vector<Utxo> input_utxos = electrum_interface.getUtxos(AddressConverter::base58_to_spkh_hex(src_addr));
    auto satoshis_needed = amount_to_transfer + satoshis_fee;
    auto utxos_funds = FundsFinder::find_funds(satoshis_needed, input_utxos);
    auto utxos = utxos_funds.first;
    auto available_funds = utxos_funds.second;
    return do_construct_p2sh_time_locking_tx_from_address(utxos, available_funds, src_addr, priv_key_ec,
                                                          amount_to_transfer, satoshis_fee, lock_until);
}

LockTxInfo OnlineLockTxCreator::do_construct_p2sh_time_locking_tx_from_address(
    const std::vector<libbitcoin::chain::output_point>& utxos,
    uint64_t available_funds,
    const string src_addr,
    const ec_private priv_key_ec,
    const uint64_t amount_to_transfer,
    const uint64_t satoshis_fee,
    const uint32_t lock_until
) {
    const wallet::ec_public pub_key = priv_key_ec.to_public();
    const libbitcoin::config::base16 priv_key = libbitcoin::config::base16(priv_key_ec.secret());
    data_chunk pub_key_data_chunk;
    pub_key.to_data(pub_key_data_chunk);
    auto satoshis_needed = amount_to_transfer + satoshis_fee;
    if (utxos.empty()){
        ostringstream oss;
        oss << "Insufficient funds, required " << satoshis_needed << ", maximum at one address available " << available_funds << "\n";
        throw InsufficientFundsException(oss.str(), satoshis_needed, available_funds);
    }
    auto refund = available_funds - satoshis_needed;

    // output 0
    script cltv_script = RedeemScript::to_pay_key_hash_pattern_with_lock(pub_key_data_chunk, lock_until);
    if(!cltv_script.is_valid()){
        throw std::invalid_argument("CLTV Script Invalid!");
    }
    short_hash script_hash = bitcoin_short_hash(cltv_script.to_data(0));
    script pay2ScriptHashLockingScript = script(cltv_script.to_pay_script_hash_pattern(script_hash));
    output output0(amount_to_transfer, pay2ScriptHashLockingScript);

    // tx
    transaction tx = transaction();
    for(auto utxo: utxos){
        input input1 = input();
        input1.set_previous_output(utxo);
        input1.set_sequence(0xfffffffe);
        tx.inputs().push_back(input1);
    }
    tx.outputs().push_back(output0);
    payment_address source_payment_address(src_addr);
    if (!source_payment_address) {
        throw InvalidAddressException("Invalid source address");
    }
    if (refund > 0){
        output output1(refund, script().to_pay_key_hash_pattern(source_payment_address.hash()));
        tx.outputs().push_back(output1);
    }
    tx.set_version(1);

    // set unlocking script in inputs
    for (unsigned int i = 0; i < utxos.size(); ++i) {
        // sig
        script previousLockingScript = script().to_pay_key_hash_pattern(bitcoin_short_hash(pub_key_data_chunk));
        endorsement sig;
        previousLockingScript.create_endorsement(sig, priv_key_ec.secret(), previousLockingScript, tx, i, all);
        // unlocking previous
        operation::list sigScript;
        sigScript.push_back(operation(sig));
        sigScript.push_back(operation(pub_key_data_chunk));
        script scriptUnlockingPreviousLockingScript(sigScript);

        tx.inputs()[i].set_script(scriptUnlockingPreviousLockingScript);
    }

    string tx_to_unlock = encode_hash(tx.hash());

    std::time_t t{lock_until};
    ostringstream infoss;
    infoss << "==========================" << "\n";
    infoss << "===== data that will be needed to unlock the funds: ====" << "\n";
    infoss << "1) lock time: " << lock_until << " <=> " << std::put_time(std::localtime(&t), "%c %Z") << "\n";
    infoss << "2) private key of address: " << src_addr << "\n";
    infoss << "3) available amount: " << amount_to_transfer << "\n";
    infoss << "   from ^^ please subtract fee" << "\n";
    infoss << "4) funding transaction id: " << tx_to_unlock << "\n";
    infoss << "5) desired target address to which the unlocked funds will be transferred" << "\n";
    infoss << "YOU NEED TO PRESERVE THIS DATA TO AVOID LOSS OF FUNDS!" << "\n";
    infoss << "==========================" << "\n";
    infoss << "==========================" << "\n";

    return LockTxInfo { encode_base16(tx.to_data()), infoss.str() };

}

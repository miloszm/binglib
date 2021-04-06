#include <binglib/bing_common.hpp>
#include <bitcoin/bitcoin.hpp>
#include "offline_unlock_tx_creator.hpp"
#include "src/libbscript/redeem_script.hpp"

using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;

string OfflineUnlockTxCreator::construct_unlock_tx(
        const string priv_key_wif,
        const string src_tx_id,
        const uint64_t satoshis_to_transfer,
        const uint32_t src_lock_until,
        const string target_addr
){
    const wallet::ec_private priv_key_ec(priv_key_wif);
    return construct_unlock_tx(priv_key_ec, src_tx_id, satoshis_to_transfer, src_lock_until, target_addr);
}

string OfflineUnlockTxCreator::construct_unlock_tx(
        const bc::wallet::ec_private priv_key_ec,
        const string src_tx_id,
        const uint64_t satoshis_to_transfer,
        const uint32_t src_lock_until,
        const string target_addr
){
    const int src_tx_output_index {0};
    const wallet::ec_public pubKey = priv_key_ec.to_public();
    const libbitcoin::config::base16 privKey = libbitcoin::config::base16(priv_key_ec.secret());
    data_chunk pubKeyChunk;
    pubKey.to_data(pubKeyChunk);

    string hashString = src_tx_id;
    hash_digest utxoHash;
    decode_hash(utxoHash, hashString);
    output_point utxo(utxoHash, src_tx_output_index);
    input input1 = input();
    input1.set_previous_output(utxo);
    input1.set_sequence(0);

    script currentLockingScript = script().to_pay_key_hash_pattern(payment_address(target_addr).hash());
    output output1(satoshis_to_transfer, currentLockingScript);

    transaction tx = transaction();
    tx.inputs().push_back(input1);
    tx.outputs().push_back(output1);
    tx.set_locktime(src_lock_until);
    tx.set_version(1);

    script redeemScript = RedeemScript::to_pay_key_hash_pattern_with_lock(pubKeyChunk, src_lock_until);
    if(!redeemScript.is_valid())
    {
        throw std::invalid_argument("CLTV Script Invalid!");
    }
    endorsement sig;
    script::create_endorsement(sig, priv_key_ec.secret(), redeemScript, tx, 0u, all);

    operation::list sigScript;
    sigScript.push_back(operation(sig));
    sigScript.push_back(redeemScript.to_data(0));
    script scriptUnlockingPreviousLockingScript(sigScript);
    tx.inputs()[0].set_script(scriptUnlockingPreviousLockingScript);

    return encode_base16(tx.to_data());
}


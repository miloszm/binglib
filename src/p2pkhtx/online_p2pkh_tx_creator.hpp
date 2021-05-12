#ifndef ONLINE_P2PKH_TX_CREATOR_HPP
#define ONLINE_P2PKH_TX_CREATOR_HPP

#include <binglib/libb_client.hpp>
#include <binglib/electrum_interface.hpp>
#include <bitcoin/bitcoin.hpp>
#include <string>

using namespace std;

class OnlineP2pkhTxCreator {
public:
    static string construct_p2pkh_tx_from_address(
            LibbClient &libb_client,
            const string src_addr,
            const string priv_key_wif,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const string target_addr
    );

    static string construct_p2pkh_tx_from_address(
            LibbClient &libb_client,
            const string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const string target_addr
    );

    static string construct_p2pkh_tx_from_address(
            ElectrumInterface &electrum_interface,
            const string src_addr,
            const string priv_key_wif,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const string target_addr
    );

    static string construct_p2pkh_tx_from_address(
            ElectrumInterface &electrum_interface,
            const string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const string target_addr
    );

private:
    static string do_construct_p2pkh_tx_from_address(
            const std::vector<libbitcoin::chain::output_point>& utxos,
            uint64_t available_funds,
            const string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const string target_addr
    );

};


#endif
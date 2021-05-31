#ifndef ONLINE_LOCK_TX_CREATOR_HPP
#define ONLINE_LOCK_TX_CREATOR_HPP

#include <binglib/bing_common.hpp>
#include <binglib/libb_client.hpp>
//#include <src/libbitcoinclient/libb_client.hpp>
#include <binglib/electrum_interface.hpp>
#include <bitcoin/system.hpp>
#include <string>

using namespace std;

struct LockTxInfo {
    string locking_tx;
    string unlocking_info;
};

class OnlineLockTxCreator {
public:
    static LockTxInfo construct_p2sh_time_locking_tx_from_address(
            LibbClient &libb_client,
            const std::string src_addr,
            const std::string priv_key_wif,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );

    static LockTxInfo construct_p2sh_time_locking_tx_from_address(
            LibbClient &libb_client,
            const std::string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );

    static LockTxInfo construct_p2sh_time_locking_tx_from_address(
            ElectrumInterface &electrum_interface,
            const std::string src_addr,
            const std::string priv_key_wif,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );

    static LockTxInfo construct_p2sh_time_locking_tx_from_address(
            ElectrumInterface &electrum_interface,
            const std::string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );
private:
    static LockTxInfo do_construct_p2sh_time_locking_tx_from_address(
            const std::vector<libbitcoin::chain::output_point>& utxos,
            uint64_t available_funds,
            const std::string src_addr,
            const bc::wallet::ec_private priv_key_ec,
            const uint64_t amount_to_transfer,
            const uint64_t satoshis_fee,
            const uint32_t lock_until
    );
};


#endif
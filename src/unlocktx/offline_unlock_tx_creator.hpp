#ifndef OFFLINE_UNLOCK_TX_CREATOR_HPP
#define OFFLINE_UNLOCK_TX_CREATOR_HPP

#include <bitcoin/system.hpp>
#include <string>

using namespace std;

class OfflineUnlockTxCreator {
public:
    static string construct_unlock_tx(
            const string priv_key_wif,
            const string src_tx_id,
            const uint64_t satoshis_to_transfer,
            const uint32_t src_lock_until,
            const string target_addr
    );

    static string construct_unlock_tx(
            const bc::wallet::ec_private priv_key_ec,
            const string src_tx_id,
            const uint64_t satoshis_to_transfer,
            const uint32_t src_lock_until,
            const string target_addr
    );
};


#endif
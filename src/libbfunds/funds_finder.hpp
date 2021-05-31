#ifndef FUNDS_FINDER_HPP
#define FUNDS_FINDER_HPP

#include <binglib/bing_common.hpp>
//#include "src/common/bing_common.hpp"
#include <bitcoin/system.hpp>
#include <binglib/electrum_model.hpp>
//#include "src/electrumclient/electrum_model.hpp"

class FundsFinder {
public:
    /**
       * Creates list of utxos to provide funds needed
       * @param satoshisNeeded
       * @param points
       * @return pair of: 1) list of utxos 2) gathered funds
       * if 1) empty means funds were not sufficient, in such case 2) contains available funds
       */
    static std::pair<std::vector<libbitcoin::chain::output_point>, uint64_t>
    find_funds(uint64_t satoshis_needed, libbitcoin::chain::points_value &points);

    /**
       * Creates list of utxos to provide funds needed
       * @param satoshisNeeded
       * @param points
       * @return pair of: 1) list of utxos 2) gathered funds
       * if 1) empty means funds were not sufficient, in such case 2) contains available funds
       */
    static std::pair<std::vector<libbitcoin::chain::output_point>, uint64_t>
    find_funds(uint64_t satoshis_needed, std::vector<Utxo> utxos);
};

#endif

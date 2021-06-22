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

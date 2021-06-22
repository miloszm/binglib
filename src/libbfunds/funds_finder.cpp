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
#include "funds_finder.hpp"

using namespace std;
using namespace bc;
using namespace bc::chain;
using namespace bc::wallet;
using namespace bc::machine;

pair<vector<output_point>, uint64_t>
FundsFinder::find_funds(uint64_t satoshis_needed, chain::points_value &points) {
    vector<output_point> utxos;
    uint64_t gathered_funds{0};
    for (auto p = begin(points.points);
         p != end(points.points) && gathered_funds < satoshis_needed; ++p) {
        if (points.value() > 0) {
            output_point utxo(p->hash(), p->index());
            utxos.push_back(utxo);
            gathered_funds += p->value();
        }
    }
    if (gathered_funds < satoshis_needed)
        return {vector<output_point>(), gathered_funds};
    else
        return {utxos, gathered_funds};
}

std::pair<std::vector<libbitcoin::chain::output_point>, uint64_t>
FundsFinder::find_funds(uint64_t satoshis_needed, std::vector<Utxo> input_utxos) {
    vector<output_point> utxos;
    uint64_t gathered_funds{0};
    for (auto p : input_utxos) {
        if (p.value > 0) {
            hash_digest tx_hash;
            decode_hash(tx_hash, p.tx_id);
            output_point utxo(tx_hash, p.tx_pos);
            utxos.push_back(utxo);
            gathered_funds += p.value;
        }
    }
    if (gathered_funds < satoshis_needed)
        return {vector<output_point>(), gathered_funds};
    else
        return {utxos, gathered_funds};
}

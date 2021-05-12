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

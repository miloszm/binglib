#ifndef REDEEM_SCRIPT_HPP
#define REDEEM_SCRIPT_HPP

//#include <binglib/bing_common.hpp>
#include "src/common/bing_common.hpp"
#include <bitcoin/system.hpp>

class RedeemScript {
public:
  static libbitcoin::machine::operation::list
  to_pay_key_hash_pattern_with_lock(const libbitcoin::data_chunk &public_key,
                                    const uint32_t lock_until);
};

#endif

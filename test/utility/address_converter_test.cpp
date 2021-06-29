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
#define BOOST_TEST_MODULE bing_test
#include <boost/test/included/unit_test.hpp>

#include "utility/address_converter.hpp"
#include "walletstate/wallet_state.hpp"
#include <iostream>

using namespace std;


bool is_address_mm(const data_slice& decoded)
{
    return (decoded.size() == payment_size) && verify_checksum(decoded);
}



BOOST_AUTO_TEST_CASE(address_2_spkh_conversion_test)
{
    string spkh_hex = AddressConverter::base58_to_spkh_hex("mpS14bFCZiHFRxfNNbnPT2FScJBrm96iLE");
    BOOST_TEST(spkh_hex == "af89af88915ddf9ee02b223800d66aec14e01bb523bd870c6c358fb935d9f004");
}

BOOST_AUTO_TEST_CASE(spkh_2_address_conversion_test)
{
    vector<string> addresses {"mpS14bFCZiHFRxfNNbnPT2FScJBrm96iLE"};
    map<string,AddressDerivationResult> address_to_data;
    WalletState wallet_state(addresses, address_to_data);
    string address = wallet_state.spkh_2_address("af89af88915ddf9ee02b223800d66aec14e01bb523bd870c6c358fb935d9f004");
    BOOST_TEST(address == "mpS14bFCZiHFRxfNNbnPT2FScJBrm96iLE");
}

BOOST_AUTO_TEST_CASE(invalid_address_conversion)
{
    payment_address invalid_payment_address_1 = payment_address("pies");
    bool valid_1{invalid_payment_address_1};
    BOOST_TEST(valid_1 == false);
    payment_address invalid_payment_address_2 = payment_address("tb1qxns7rtlkql56lp6mt2dmp79yrumnmja66t052d");
    bool valid_2{invalid_payment_address_2};
    BOOST_TEST(valid_2 == false);
}

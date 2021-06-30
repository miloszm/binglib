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
#ifndef BING_EXCEPTION_HPP
#define BING_EXCEPTION_HPP

#include <string>
#include <bitcoin/system.hpp>

using namespace std;

class InsufficientFundsException : public std::exception {
public:
    std::string message_;
    uint64_t needed_amount_;
    uint64_t available_amount_;

    explicit InsufficientFundsException(const std::string& message, uint64_t needed_amount, uint64_t available_amount)
        : message_(message), needed_amount_(needed_amount), available_amount_(available_amount) {}

    virtual ~InsufficientFundsException() noexcept {}

    virtual const char* what() const noexcept {
        return message_.c_str();
    }
};

class InvalidTargetAddressException : public std::exception {
public:
    explicit InvalidTargetAddressException(){}

    virtual ~InvalidTargetAddressException() noexcept {}

    virtual const char* what() const noexcept {
        return "invalid target address";
    }
};

class InvalidSourceAddressException : public std::exception {
public:
    explicit InvalidSourceAddressException(){}

    virtual ~InvalidSourceAddressException() noexcept {}

    virtual const char* what() const noexcept {
        return "invalid source address";
    }
};

#endif

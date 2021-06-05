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

#endif

#ifndef ADDRESS_CONVERTER_HPP
#define ADDRESS_CONVERTER_HPP

#include <string>

class AddressConverter {
public:
    static std::string base58_to_spkh_hex(std::string base58_address);
};

#endif

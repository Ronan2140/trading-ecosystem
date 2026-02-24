//
// Created by ronan2140 on 2/23/26.
//

#ifndef TRADING_ECOSYSTEM_NETWORKMESSAGES_HPP
#define TRADING_ECOSYSTEM_NETWORKMESSAGES_HPP
#include <cstdint>

#pragma pack(push, 1)

// need a 32 bit struct (no padding for cpu)
struct InboundOrder {
    // order id 8 bytes
    uint64_t  orderID;
    // price 8 bytes
    uint64_t price; // if the price is 123.45, 12345 will be stocked
    // action name 8 byte
    char symbol[8];

    // quantity 4 bytes
    uint32_t quantity;
    // bid or ask 1 byte
    char side;

    // 3 bytes free, padding added manally so the cpu treat the info faster
    char padding[3];

};

#pragma pack(pop)

#endif //TRADING_ECOSYSTEM_NETWORKMESSAGES_HPP

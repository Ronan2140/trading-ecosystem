//
// Created by ronan2140 on 2/23/26.
//

#ifndef TRADING_ECOSYSTEM_ORDER_HPP
#define TRADING_ECOSYSTEM_ORDER_HPP
#include <cstdint>

enum class Side : uint8_t {
    BID = 0,
    ASK = 1
};

// Total size = 41 byte -> padding 48 bytes
class Order {
    public:

        // 8 bytes items
        uint64_t  orderID;
        uint64_t price; // if the price is 123.45, 12345 will be stocked
        uint64_t quantity; // if the quantity is 2.01, 201 will be stocked+
        Order* next;
        Order* previous;

        // 1 byte items
        Side side;

    void reset(const uint64_t id, const uint64_t newPrice, const uint64_t newQuantity, const Side newSide) {
        this->orderID = id;
        this->price = newPrice;
        this->quantity = newQuantity;
        this->side = newSide;
        next = nullptr;
        previous = nullptr;
    }

};

#endif //TRADING_ECOSYSTEM_ORDER_HPP

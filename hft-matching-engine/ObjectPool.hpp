//
// Created by ronan2140 on 2/23/26.
//

#ifndef TRADING_ECOSYSTEM_OBJECTPOOL_HPP
#define TRADING_ECOSYSTEM_OBJECTPOOL_HPP
#include "Order.hpp"


class ObjectPool {
    Order* objectPool;
    Order* firstFreeOrder;
    std::size_t objectPoolSize;

public:
    ObjectPool(std::size_t objectPoolSize) {
        objectPool = new Order[objectPoolSize];
        firstFreeOrder = &objectPool[0];
        this->objectPoolSize = objectPoolSize;
        for (std::size_t i = 0; i < objectPoolSize - 1 ; i++) {
            objectPool[i].next = &objectPool[i + 1];
        }
        objectPool[objectPoolSize - 1].next = nullptr;
    }

    Order* acquire() {
        if (!firstFreeOrder) return nullptr;
        Order* currentFirstOrder = firstFreeOrder;
        firstFreeOrder = firstFreeOrder->next;

        currentFirstOrder->next = nullptr;
        currentFirstOrder->previous = nullptr;
        return currentFirstOrder;
    }

    void release(Order* order) {
        order->next = firstFreeOrder;
        firstFreeOrder = order;
    }
};
#endif //TRADING_ECOSYSTEM_OBJECTPOOL_HPP

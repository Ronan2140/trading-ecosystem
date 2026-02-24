//
// Created by ronan2140 on 2/23/26.
//

#ifndef TRADING_ECOSYSTEM_ORDERBOOK_HPP
#define TRADING_ECOSYSTEM_ORDERBOOK_HPP
#include <cstdint>
#include <vector>

#include "OrderQueue.hpp"
class OrderBook {
    std::vector<OrderQueue> bids = {};
    std::vector<OrderQueue> asks = {};
    uint64_t highestBidsPrice = 0;
    uint64_t lowestAsksPrice = -1; // unsinged int will go maxint if -1 is put, so any sell will be lower
    uint64_t max_ask_seen = 0;
    uint64_t min_bid_seen = 1'000'000;
    ObjectPool& pool;

public :
    OrderBook(ObjectPool& pool) : pool(pool) {
            bids.resize(1'000'000);
            asks.resize(1'000'000);
    }
    void addOrder(Order* order) {
            if (order->side == Side::BID) {
                if (order->price > highestBidsPrice) {
                    highestBidsPrice = order->price;
                }
                if (order->price < min_bid_seen) {
                    min_bid_seen = order->price;
                }
                bids[order->price].pushBack(order);
            }
            if (order->side == Side::ASK) {
                if (order->price < lowestAsksPrice) {
                    lowestAsksPrice = order->price;
                }
                if (order->price > max_ask_seen) {
                    max_ask_seen = order->price;
                }
                asks[order->price].pushBack(order);
            }
    }
    void cancelOrder(Order* order) {
        if (order->side == Side::BID) {
            if (bids[order->price].remove(order) && order->price == highestBidsPrice) {
                // Look for the next best bid
                if (highestBidsPrice == 0) {
                    return;
                }
                for (uint64_t i = highestBidsPrice - 1; i > min_bid_seen; i--) {
                    if (!bids[i].isEmpty()) {
                        highestBidsPrice = i;
                        return;
                    }
                }
                highestBidsPrice = 0;
                return;
            }
        }
        if (order->side == Side::ASK) {
            if (asks[order->price].remove(order) && order->price == lowestAsksPrice) {
                // Look for the next best ask
                for (uint64_t i = lowestAsksPrice + 1; i < max_ask_seen ; i++) {
                    if (!asks[i].isEmpty()) {
                        lowestAsksPrice = i;
                        return;
                    }
                }
                lowestAsksPrice = -1;
            }
        }
        pool.release(order);
    }

    void matchOrder(Order* incomingOrder, const bool silentMode=false) {
        if (incomingOrder->side == Side::BID) {
            while (incomingOrder->price >= lowestAsksPrice && incomingOrder->quantity > 0) {
                OrderQueue& targetQueue = asks[lowestAsksPrice];
                Order* restingOrder = targetQueue.getHead();
                if (!restingOrder) break;
                const uint64_t sharedQuantity = std::min(restingOrder->quantity,
                    incomingOrder->quantity);

                if (!silentMode)
                    std::cout << "TRADE: " << sharedQuantity << " @ " << lowestAsksPrice << std::endl;

                incomingOrder->quantity -= sharedQuantity;
                restingOrder->quantity -= sharedQuantity;
                if (restingOrder->quantity == 0) {
                    cancelOrder(restingOrder);
                }

            }

            if (incomingOrder->quantity > 0) {
                this->addOrder(incomingOrder);
            }
            else {
                pool.release(incomingOrder);
            }

        }
        else {
            while (incomingOrder->price <= highestBidsPrice && incomingOrder->quantity > 0) {
                OrderQueue& targetQueue = bids[highestBidsPrice];
                Order* restingOrder = targetQueue.getHead();
                if (!restingOrder) break;
                const uint64_t sharedQuantity = std::min(restingOrder->quantity,
                    incomingOrder->quantity);
                if (!silentMode)
                    std::cout << "TRADE: " << sharedQuantity << " @ " << highestBidsPrice << std::endl;

                incomingOrder->quantity -= sharedQuantity;
                restingOrder->quantity -= sharedQuantity;
                if (restingOrder->quantity == 0) {
                    cancelOrder(restingOrder);
                }

            }

            if (incomingOrder->quantity > 0) {
                this->addOrder(incomingOrder);
            }
            else {
                pool.release(incomingOrder);
            }
        }
    }



  [[nodiscard]] uint64_t highest_bids_price() const {
        return highestBidsPrice;
    }

    void set_highest_bids_price(uint64_t highest_bids_price) {
        highestBidsPrice = highest_bids_price;
    }

    [[nodiscard]] uint64_t lowest_asks_price() const {
        return lowestAsksPrice;
    }

    void set_lowest_asks_price(uint64_t lowest_asks_price) {
        lowestAsksPrice = lowest_asks_price;
    }
};

#endif //TRADING_ECOSYSTEM_ORDERBOOK_HPP

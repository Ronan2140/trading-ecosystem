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
    std::vector<uint64_t> bids_bitset;
    std::vector<uint64_t> asks_bitset;
    uint64_t highestBidsPrice = 0;
    uint64_t lowestAsksPrice = -1; // unsinged int will go maxint if -1 is put, so any sell will be lower
    uint64_t max_ask_seen = 0;
    uint64_t min_bid_seen = 1'000'000;
    ObjectPool& pool;

public :
    OrderBook(ObjectPool& pool) : pool(pool), bids_bitset((1'000'000 / 64) + 1, 0), asks_bitset((1'000'000 / 64) + 1, 0) {
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
                activateBitBids(order->price);
            }
            if (order->side == Side::ASK) {
                if (order->price < lowestAsksPrice) {
                    lowestAsksPrice = order->price;
                }
                if (order->price > max_ask_seen) {
                    max_ask_seen = order->price;
                }
                asks[order->price].pushBack(order);
                activateBitAsks(order->price);
            }
    }

    void cancelOrder(Order* order) {
        if (order->side == Side::BID) {
            if (bids[order->price].remove(order) && order->price == highestBidsPrice) {
                if (highestBidsPrice == 0) {
                    return;
                }
                deactivateBitBids(order->price);

                // Look for the next best bid
                // Huge optimization point
                /* for (uint64_t i = highestBidsPrice - 1; i > min_bid_seen; i--) {
                    if (!bids[i].isEmpty()) {
                        highestBidsPrice = i;
                        return;
                    }
                } */

                const auto currentWordIdx = static_cast<int64_t>(highestBidsPrice / 64);
                const auto minWordIdx = static_cast<int64_t>(min_bid_seen / 64);
                // version bitset
                for (int64_t i = currentWordIdx; i >= minWordIdx; --i) {
                    // read the word
                    if (bids_bitset[i] != 0) {
                        // get the first non nul bit
                        const uint64_t highestBitInWord = 63 - __builtin_clzll(bids_bitset[i]);
                        highestBidsPrice = static_cast<uint64_t>(i) * 64 + highestBitInWord;
                        return;
                    }
                }
                highestBidsPrice = 0;
                return;
            }
        }
        if (order->side == Side::ASK) {
            if (asks[order->price].remove(order) && order->price == lowestAsksPrice) {
                deactivateBitAsks(order->price);
                // Look for the next best ask
                // Huge optimization point
                /* for (uint64_t i = lowestAsksPrice + 1; i < max_ask_seen ; i++) {
                    if (!asks[i].isEmpty()) {
                        lowestAsksPrice = i;
                        return;
                    }
                } */

                const uint64_t currentWordIdx = lowestAsksPrice / 64;
                const uint64_t maxWordIdx = max_ask_seen / 64;
                // version bitset
                for (uint64_t i = currentWordIdx; i <= maxWordIdx; i++) {
                    // read the word
                    if (asks_bitset[i] != 0) {
                        // get the first non nul bit
                        const uint64_t lowestBitInWord = __builtin_ctzll(asks_bitset[i]);
                        lowestAsksPrice = i * 64 + lowestBitInWord;
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
    void activateBitBids(uint64_t price) {
        uint64_t indexInList = price / 64;
        uint64_t indexInWord = price % 64;
        // set the bit to 1
        bids_bitset[indexInList] |= (1ULL << indexInWord);
    }
    void deactivateBitBids(uint64_t price) {
        if (bids[price].isEmpty()) {
            bids_bitset[price / 64] &= ~(1ULL << (price % 64));
        }

    }
    void activateBitAsks(uint64_t price) {
        uint64_t indexInList = price / 64;
        uint64_t indexInWord = price % 64;
        // set the bit to 1
        asks_bitset[indexInList] |= (1ULL << indexInWord);
    }
    void deactivateBitAsks(uint64_t price) {
        if (asks[price].isEmpty()) {
            asks_bitset[price / 64] &= ~(1ULL << (price % 64));
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

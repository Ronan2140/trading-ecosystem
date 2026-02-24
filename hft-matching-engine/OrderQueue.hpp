//
// Created by ronan2140 on 2/23/26.
//

#ifndef TRADING_ECOSYSTEM_ORDERQUEUE_HPP
#define TRADING_ECOSYSTEM_ORDERQUEUE_HPP
#include "Order.hpp"

class OrderQueue {
    Order* head = nullptr;
    Order* tail = nullptr;

    public:
        void pushBack(Order* newOrder) {
            if (this->tail == nullptr) {
                this->head = newOrder;
                this->tail = newOrder;
            }
            else {
                this->tail->next = newOrder;
                newOrder->previous = this->tail;
                this->tail = newOrder;
            }

        };

        /**
         * @brief Remove a given order from the order Queue,
         * and return whether the order queue is now empty or not
         * @return bool True if the OrderQueue is now empty
         **/
        bool remove(Order* order) {
            if (order->previous) {
                order->previous->next = order->next;
            }
            else {
                head = order->next;
            }
            if (order->next) {
                order->next->previous = order->previous;
            }
            else {
                tail = order->previous;
            }
            order->next = nullptr;
            order->previous = nullptr;
            return head == nullptr;
        }

        bool isEmpty() {
            return this->head == nullptr;
        }

        Order* getHead() {
            return this->head;
        }
};
#endif //TRADING_ECOSYSTEM_ORDERQUEUE_HPP

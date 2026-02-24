//
// Created by ronan2140 on 2/24/26.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../hft-matching-engine/ObjectPool.hpp"
#include "../hft-matching-engine/OrderBook.hpp"


TEST_CASE("Matching Engine - Basic Execution", "[matching]") {
    ObjectPool pool(100);
    OrderBook book(pool);

    SECTION("Full execution of two matching orders") {
        Order* s1 = pool.acquire();
        s1->reset(1, 100, 50, Side::ASK);
        book.matchOrder(s1);

        Order* b1 = pool.acquire();
        b1->reset(2, 100, 50, Side::BID);
        book.matchOrder(b1);

        REQUIRE(s1->quantity == 0);
        REQUIRE(b1->quantity == 0);
    }

    SECTION("Partial execution") {
        Order* s1 = pool.acquire();
        s1->reset(1, 100, 100, Side::ASK);
        book.matchOrder(s1);

        Order* b1 = pool.acquire();
        b1->reset(2, 100, 40, Side::BID);
        book.matchOrder(b1);

        REQUIRE(s1->quantity == 60);
        REQUIRE(b1->quantity == 0);
    }
}

TEST_CASE("Passive Orders - No Match", "[orderbook]") {
    ObjectPool pool(10);
    OrderBook book(pool);

    SECTION("Buy order becomes resting") {
        Order* b1 = pool.acquire();
        b1->reset(1, 100, 10, Side::BID);
        book.matchOrder(b1);

        CHECK(b1->quantity == 10);

        CHECK(book.highest_bids_price() == 100);
    }

    SECTION("Sell order becomes resting") {
        Order* s1 = pool.acquire();
        s1->reset(2, 200, 10, Side::ASK);
        book.matchOrder(s1);

        CHECK(s1->quantity == 10);
    }
    SECTION("One big buyer vs many small sellers") {
        // 3 vendeurs de 10 à 100
        for(int i=0; i<3; ++i) {
            Order* s = pool.acquire();
            s->reset(i, 100, 10, Side::ASK);
            book.matchOrder(s);
        }
        // 1 acheteur de 25 à 100
        Order* b = pool.acquire();
        b->reset(4, 100, 25, Side::BID);
        book.matchOrder(b);

        CHECK(b->quantity == 0);
        // Il doit rester 5 dans le dernier vendeur (le 3ème)
    }

    SECTION("Price level depletion") {
        Order* b1 = pool.acquire(); b1->reset(1, 100, 10, Side::BID); book.matchOrder(b1);
        Order* b2 = pool.acquire(); b2->reset(2, 90, 10, Side::BID); book.matchOrder(b2);

        CHECK(book.highest_bids_price() == 100);

        // Un vendeur vide le niveau 100
        Order* s1 = pool.acquire(); s1->reset(3, 100, 10, Side::ASK); book.matchOrder(s1);

        // Le prix max doit maintenant être 90
        CHECK(book.highest_bids_price() == 90);
    }
}

    TEST_CASE("Object Pool - Recycling stability", "[memory]") {
        // little pool
        ObjectPool pool(2);
        OrderBook book(pool);

        SECTION("Continuous Buy/Sell cycle") {
            for (int i = 0; i < 1000; ++i) {
                // 1. Un vendeur arrive
                Order* s = pool.acquire();
                REQUIRE(s != nullptr); // Si c'est nullptr, le pool est plein (fuite !)
                s->reset(i, 100, 10, Side::ASK);
                book.matchOrder(s);

                // 2. Un acheteur arrive et match tout
                Order* b = pool.acquire();
                REQUIRE(b != nullptr);
                b->reset(i + 1000, 100, 10, Side::BID);
                book.matchOrder(b);

                // match order shoud :
                // pool.release(s) et pool.release(b) because qty == 0
            }
            SUCCEED("1000 order with pool of 2 !");
        }
        SECTION("Pool Exhaustion") {
            ObjectPool smallPool(1);
            Order* first = smallPool.acquire();
            Order* second = smallPool.acquire();

            CHECK(first != nullptr);
            CHECK(second == nullptr);
        }

}
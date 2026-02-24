//
// Created by ronan2140 on 2/24/26.
//


#include <chrono>
#include <iostream>
#include <vector>


#include "ObjectPool.hpp"
#include "OrderBook.hpp"

void runBench() {
    constexpr int num_orders = 1'000'000;
    ObjectPool pool(num_orders + 10);
    OrderBook book(pool);
    std::cout << "Preparing benchmark..." << std::endl;
    // start the timer
    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_orders; i++) {
        Order* s = pool.acquire();
        if (i % 2 == 0) {
            s->reset(i, 999'900, 10, Side::ASK);
        } else {
            s->reset(i, 999'900, 10, Side::BID);
        }
        book.matchOrder(s, true);

    }
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> diff = end - start;

    const double orders_per_second = num_orders / diff.count();
    const double avg_latency_ns = (diff.count() * 1e9) / num_orders;

    std::cout << "=== BENCHMARK RESULTS ===" << std::endl;
    std::cout << "Total Orders: " << num_orders << std::endl;
    std::cout << "Total Time  : " << diff.count() << " seconds" << std::endl;
    std::cout << "Throughput  : " << orders_per_second << " orders/sec" << std::endl;
    std::cout << "Avg Latency : " << avg_latency_ns << " ns/order" << std::endl;
    std::cout << "=========================" << std::endl;
}

int main() {
    std::cout << "--- Starting High Speed Benchmark ---" << std::endl;
    runBench();
    return 0;
}

//
// Created by ronan2140 on 2/24/26.
//


#include <chrono>
#include <iostream>
#include <random>
#include <vector>


#include "ObjectPool.hpp"
#include "OrderBook.hpp"

void runBench() {
    constexpr uint64_t num_orders = 1'000'000;
    ObjectPool pool(1000);
    OrderBook book(pool);
    std::cout << "Preparing benchmark..." << std::endl;
    // start the timer
    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_orders; i++) {
        Order* s = pool.acquire();
        if (i % 2 == 0) {
            s->reset(i, 100, 10, Side::ASK);
        } else {
            s->reset(i, 100, 10, Side::BID);
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


void runBenchRandom() {
    constexpr uint64_t num_orders = 1'000'000;
    ObjectPool pool(num_orders); // On augmente le pool pour stocker les ordres qui ne matchent pas
    OrderBook book(pool);

    // Initialisation du générateur aléatoire
    std::random_device rd;
    std::mt19937 gen(rd());

    // Distribution des prix : entre 90 et 110
    std::uniform_int_distribution<uint64_t> priceDist(90, 1000);
    // Distribution des quantités : entre 1 et 100
    std::uniform_int_distribution<uint32_t> qtyDist(1, 500);
    // Distribution Side : 50/50
    std::uniform_int_distribution<int> sideDist(0, 1);

    std::cout << "Starting Random Benchmark (1M orders)..." << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < num_orders; i++) {
        Order* o = pool.acquire();
        if (!o) continue; // Sécurité si le pool est plein

        uint64_t price = priceDist(gen);
        uint32_t qty = qtyDist(gen);
        Side side = (sideDist(gen) == 0) ? Side::BID : Side::ASK;

        o->reset(i, price, qty, side);

        // Le deuxième argument 'true' pour le mode silencieux est crucial !
        book.matchOrder(o, true);
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
    runBenchRandom();
    return 0;
}

//
// Created by ronan2140 on 2/23/26.
//

#ifndef TRADING_ECOSYSTEM_TCPSERVER_HPP
#define TRADING_ECOSYSTEM_TCPSERVER_HPP
#include "../hft-matching-engine/ObjectPool.hpp"
#include "../hft-matching-engine/OrderBook.hpp"
#include <sys/socket.h> // Pour socket(), bind(), listen(), accept()
#include <netinet/in.h> // Pour sockaddr_in (les adresses IP)
#include <fcntl.h>       // Pour fcntl() (le mode non-bloquant)
#include <unistd.h>      // Pour read(), write(), close()
#include <iostream>

#include "NetworkMessages.hpp"


class TcpServer {
private:
    int server_fd; // file descritpor for the socket
    int client_fd = -1;
    OrderBook& orderBook;
    ObjectPool& objectPool;


public :
    TcpServer(OrderBook& ob, ObjectPool& p, int port) : orderBook(ob), objectPool(p) {

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    constexpr int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // listen from any
    address.sin_port = htons(port);       // Port 8080
    bind(server_fd, reinterpret_cast<sockaddr *>(&address), sizeof(address));
    listen(server_fd, 3);

    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    std::cout << "HTF Server listening on " << port << std::endl;

    }

    void update() {

        if (client_fd == -1) {
            client_fd = accept(server_fd, nullptr, nullptr);
            if (client_fd != -1) {
                fcntl(client_fd, F_SETFL, O_NONBLOCK); // Indispensable !
                std::cout << "Connected with client fd " << client_fd << std::endl;
            }
        }
        if (client_fd != -1) {
            char buffer[32];
            if (const ssize_t valRead = read(client_fd, buffer, 32); valRead == 32) {
                const auto* inboundOrder = reinterpret_cast<InboundOrder*>(buffer);
                std::cout << "Ordre recu - ID: " << inboundOrder->orderID
                                          << " Prix: " << inboundOrder->price << std::endl;
                if (Order* order = objectPool.acquire())
                {
                    order->reset(inboundOrder->orderID, inboundOrder->price, inboundOrder->quantity, (inboundOrder->side == 'A') ? Side::ASK : Side::BID);
                    orderBook.matchOrder(order);
                    std::cout << "Ordre converti - ID: " << order->orderID
                                                              << " Prix: " << order->price << std::endl;
                }

            }
            else if (valRead == 0) {
                // disconnected
                close(client_fd);
                client_fd = -1;
            }
        }





    }
};

#endif //TRADING_ECOSYSTEM_TCPSERVER_HPP

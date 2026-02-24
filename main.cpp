#include <iostream>
#include <cstdint>

#include "hft-matching-engine/ObjectPool.hpp"
#include "hft-matching-engine/Order.hpp"
#include "hft-matching-engine/OrderBook.hpp"
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
#include "hft-matching-engine/OrderBook.hpp"
#include "hft-matching-engine/ObjectPool.hpp"
#include "market-connectivity/TcpServer.hpp"
#include <csignal>

// Variable pour arrêter proprement le serveur avec Ctrl+C
bool keepRunning = true;
void signalHandler(int signum) { keepRunning = false; }

int main() {
    // 1. Initialisation des composants (1 million d'ordres pré-alloués)
    ObjectPool pool(1000000);
    OrderBook book(pool);

    // 2. Lancement du serveur sur le port 8080
    TcpServer server(book, pool, 8080);

    // Gestion propre de l'arrêt
    signal(SIGINT, signalHandler);

    std::cout << "Moteur de trading prêt. En attente d'ordres..." << std::endl;

    // 3. LA BOUCLE HFT (Spin-loop)
    // Elle tourne à 100% du CPU pour être la plus réactive possible
    while (keepRunning) {
        server.update();

        // Optionnel : on peut ajouter un micro-sommeil pour ne pas
        // faire chauffer le processeur inutilement pendant les tests
        usleep(1);
    }

    std::cout << "Arrêt du serveur..." << std::endl;
    return 0;
}
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.

# 📈 HFT Matching Engine & Market Connectivity

🌍 **Language / Langue :** [English](#-english-version) | [Français](#-version-française)

---

## 🇬🇧 English Version

A ultra-low latency High-Frequency Trading (HFT) matching engine built in modern C++. This project simulates a professional exchange core, focusing on memory efficiency and extreme execution speed.

### 🚀 Features
* **Zero-Allocation Critical Path**: Custom `ObjectPool` pre-allocates 1,000,000 `Order` objects to eliminate runtime `new/delete` overhead.
* **Price-Time Priority**: Efficient `OrderBook` using intrusive linked lists (`OrderQueue`) for O(1) order insertion and removal.
* **Binary Protocol**: Strict 32-byte packed messages (`InboundOrder`) for optimal CPU cache alignment.
* **Non-Blocking Network**: `TcpServer` with a reactive spin-loop using `fcntl` non-blocking sockets.

### 📊 Performance Benchmarks
*Measured on a standard CPU with 1M random orders (-O3 Release build):*
* **Throughput**: ~8 Million orders/sec
* **Average Latency**: **124.404 ns** / order

## 📁 Project Structure

```text
.
├── bench/                  # Performance benchmarking tools
├── hft-matching-engine/    # Core logic: OrderBook, OrderQueue, ObjectPool
├── market-connectivity/    # Network layer: TcpServer and Binary Messages
├── tests/                  # Unit tests (Catch2)
├── main.cpp                # Application entry point
├── trader.py               # Python test client to inject orders
└── CMakeLists.txt          # CMake build configuration
```

### 🛠️ Build & Run (Optimized Benchmark)
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto -DNDEBUG" ..
make -j$(nproc)
```

2. Run the HFT Server:
The server runs a continuous high-frequency spin-loop to process orders with minimal latency.

```bash
./trading_ecosystem
# The server will start listening for TCP connections on port 8080.
# Press Ctrl+C to stop it gracefully.
```

3. Run Unit Tests (Catch2):
To verify the core logic (OrderBook price-time priority, ObjectPool recycling, etc.):

```bash
./unit_tests
```

4. Run Performance Benchmarks:
To measure the engine's throughput and average latency on your specific hardware:

```bash
./bench_engine
```

5. Simulate Order Flow (Python Client):
Open a new terminal window at the root of the project and run the simulated trader to inject binary orders into the engine.

```bash
python3 trader.py
```

## 🇫🇷 Version Française

# 📈 Moteur de Matching HFT & Connectivité Marché

Ce projet est un moteur de matching d'ordres à ultra-faible latence (High-Frequency Trading) développé en C++ moderne. Il simule le cœur d'un exchange professionnel en se concentrant sur l'efficacité mémoire et une vitesse d'exécution extrême.

---

## 🚀 Fonctionnalités Clés

* **Chemin Critique sans Allocation** : Utilisation d'un `ObjectPool` personnalisé qui pré-alloue 1 000 000 d'objets `Order`. Cela élimine totalement les appels `new`/`delete` (très coûteux) pendant la phase de matching.
* **Priorité Prix-Temps** : Un `OrderBook` ultra-rapide utilisant des listes chaînées intrusives (`OrderQueue`) pour garantir des insertions et des suppressions d'ordres en temps constant $O(1)$.
* **Protocole Binaire Optimisé** : Les messages réseau (`InboundOrder`) sont des structures compactes de 32 octets, sans padding inutile, optimisées pour l'alignement du cache CPU (L1/L2).
* **Réseau Non-Bloquant** : Un `TcpServer` utilisant une "spin-loop" réactive et des sockets non-bloquants (`fcntl`) pour une ingestion des ordres avec une gigue (jitter) minimale.

---

## 📊 Performances (Benchmarks)

Les résultats ci-dessous ont été obtenus sur une machine standard avec un flux de **1 000 000 d'ordres aléatoires** (Build Release avec optimisations `-O3`) :

* **Débit (Throughput)** : ~8 Millions d'ordres / seconde
* **Latence Moyenne** : **124.404 ns** par ordre

> **Note importante** : En mode `Debug` (non-optimisé), la latence peut monter à ~250 000 ns. Utilisez toujours le profil `Release` pour mesurer les performances réelles.

---

## 📁 Architecture du Projet

```text
.
├── hft-matching-engine/    # Logique cœur (OrderBook, ObjectPool, Order)
├── market-connectivity/    # Serveur TCP & Protocole binaire
├── bench/                  # Outils de mesure de performances
├── tests/                  # Tests unitaires complets (Catch2)
├── main.cpp                # Point d'entrée du serveur
└── trader.py               # Client de simulation en Python
└── CMakeLists.txt          # CMake de configuration

### 🛠️ Compilation et Exécution (Optimized Benchmark)
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto -DNDEBUG" ..
make -j$(nproc)
```

2. Lancement du Serveur :
Le serveur utilise une boucle HFT (spin-loop) continue pour traiter les ordres avec une latence réseau minimale.

```bash
./trading_ecosystem
# Le serveur écoutera les connexions TCP sur le port 8080. 
# Appuyez sur Ctrl+C pour l'arrêter proprement.
```

3. Exécution des Tests Unitaires :
Pour vérifier l'intégrité de la logique métier (priorité du carnet d'ordres, recyclage de la mémoire par le pool, etc.) via Catch2 :

```bash
./unit_tests
```

4. Lancement du Benchmark de Performance :
Pour mesurer le débit (throughput) et la latence moyenne directement sur votre machine :

```bash
./bench_engine
```

5. Simulation d'Ordres (Client Python) :
Ouvrez un nouveau terminal à la racine du projet et lancez le trader simulé pour injecter des ordres binaires dans le moteur.

```bash
python3 trader.py
```






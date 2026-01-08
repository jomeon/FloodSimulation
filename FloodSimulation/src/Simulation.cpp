#include "Simulation.h"
#include <iostream>
#include <chrono>

Simulation::Simulation(int gridSize, int iter)
    : N(gridSize), iterations(iter), grid(gridSize) {
    grid.generateTerrain();
    grid.setSources(3); // 3 Ÿród³a rzek
}

void Simulation::run() {
    std::vector<std::thread> threads;
    std::barrier syncPoint(N);

    std::cout << "Start symulacji na " << N << " watkach..." << std::endl;

    for (int i = 0; i < N; ++i) {
        threads.emplace_back(&Simulation::workerThread, this, i, std::ref(syncPoint));
    }

    for (auto& t : threads) {
        t.join();
    }
    std::cout << "Symulacja zakonczona." << std::endl;
}

void Simulation::workerThread(int r, std::barrier<>& syncPoint) {
    // Lokalny generator liczb losowych dla w¹tku
    std::mt19937 gen(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::uniform_real_distribution<> rainDist(0.0, 5.0);
    std::uniform_real_distribution<> probDist(0.0, 1.0);

    for (int iter = 0; iter < iterations; ++iter) {

        for (int c = 0; c < N; ++c) {
            Cell& cell = grid.getCell(r, c);

            // ród³a sta³e
            if (cell.is_source) cell.water += 1.0;

            // Topnienie œniegu
            if (cell.height > snowLevel) {
                cell.water += (cell.height - snowLevel) * 0.01;
            }

            // Losowy deszcz
            if (probDist(gen) < rainProbability) {
                cell.water += rainDist(gen);
            }
        }

        syncPoint.arrive_and_wait();

        for (int c = 0; c < N; ++c) {
            calculateFlow(r, c);
        }

        syncPoint.arrive_and_wait();

        for (int c = 0; c < N; ++c) {
            grid.getCell(r, c).updateRisk();
        }

        syncPoint.arrive_and_wait();
    }
}

void Simulation::calculateFlow(int r, int c) {
    Cell& current = grid.getCell(r, c);

    if (current.water <= 0.001) return;

    struct Neighbor { int r, c; double h; };
    std::vector<Neighbor> neighbors;
    double minHeight = current.height;

    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr;
            int nc = c + dc;

            if (nr >= 0 && nr < N && nc >= 0 && nc < N) {
                double nh = grid.getCell(nr, nc).height;
                if (nh < current.height) { 
                    neighbors.push_back({ nr, nc, nh });
                    if (nh < minHeight) minHeight = nh;
                }
            }
        }
    }

    if (neighbors.empty()) return; 

    std::vector<Neighbor> lowestNeighbors;
    for (const auto& n : neighbors) {
        if (std::abs(n.h - minHeight) < 0.001) {
            lowestNeighbors.push_back(n);
        }
    }

    if (lowestNeighbors.empty()) return;

    double heightDiff = current.height - minHeight;
    double flowAmount = current.water * 0.5 * (heightDiff / 100.0);
    if (flowAmount > current.water) flowAmount = current.water;

    double waterPerNeighbor = flowAmount / lowestNeighbors.size();

    {
        std::lock_guard<std::mutex> lock(grid.getRowMutex(r));
        current.water -= flowAmount;
        current.flow_out = flowAmount;
    }

    for (const auto& n : lowestNeighbors) {
        grid.safeAddWater(n.r, n.c, waterPerNeighbor);
    }
}

void Simulation::printResults() {
    grid.printTopRisks(10);
    grid.saveToCSV("wyniki.csv");
}
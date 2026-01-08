#include <iostream>
#include "src/Simulation.h"

int main() {
    int N, iterations;

    std::cout << "--- Symulacja Systemu Rzecznego ---\n";
    std::cout << "Podaj rozmiar siatki N (np. 20): ";
    if (!(std::cin >> N) || N < 5) N = 20;

    std::cout << "Podaj liczbe iteracji (np. 100): ";
    if (!(std::cin >> iterations) || iterations < 1) iterations = 50;

    try {
        Simulation sim(N, iterations);
        sim.run();
        sim.printResults();
    }
    catch (const std::exception& e) {
        std::cerr << "Blad krytyczny: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
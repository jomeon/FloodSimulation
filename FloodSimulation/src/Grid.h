#pragma once
#include <vector>
#include <mutex>
#include <random>
#include "Cell.h"

class Grid {
public:
    Grid(int size);

    int getSize() const { return N; }

    // Dostêp do komórki (niebezpieczny bez mutexa w kontekœcie wielow¹tkowym)
    Cell& getCell(int row, int col);

    // Bezpieczne dodanie wody (blokuje mutex odpowiedniego wiersza)
    void safeAddWater(int row, int col, double amount);

    // Inicjalizacja terenu
    void generateTerrain();
    void setSources(int count);

    // Dostêp do mutexa wiersza (dla logiki symulacji)
    std::mutex& getRowMutex(int row);

    // Wyœwietlanie wyników
    void printWaterMap() const;
    void printTopRisks(int limit) const;

    void saveToCSV(const std::string& filename) const;

private:
    int N;
    std::vector<std::vector<Cell>> cells;

    // Mutex dla ka¿dego wiersza - zapobiega wyœcigom przy zapisie do s¹siadów
    // U¿ywamy unique_ptr, aby wektor by³ przesuwalny (mutexy nie s¹ kopiowalne)
    std::vector<std::unique_ptr<std::mutex>> rowMutexes;
};
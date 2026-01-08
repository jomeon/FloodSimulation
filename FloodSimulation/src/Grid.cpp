#include "Grid.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>

Grid::Grid(int size) : N(size) {
    cells.resize(N, std::vector<Cell>(N));
    for (int i = 0; i < N; ++i) {
        rowMutexes.push_back(std::make_unique<std::mutex>());
    }
}

Cell& Grid::getCell(int row, int col) {
    return cells[row][col];
}

std::mutex& Grid::getRowMutex(int row) {
    return *rowMutexes[row];
}

void Grid::safeAddWater(int row, int col, double amount) {
    std::lock_guard<std::mutex> lock(*rowMutexes[row]);
    cells[row][col].water += amount;
}

void Grid::generateTerrain() {
    std::mt19937 gen(1234); 
    std::uniform_real_distribution<> heightDist(10.0, 100.0);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double distFromCenter = std::sqrt(std::pow(i - N / 2.0, 2) + std::pow(j - N / 2.0, 2));
            cells[i][j].height = 100.0 - (distFromCenter * 2.0);
            if (cells[i][j].height < 0) cells[i][j].height = 5.0;

            // Losowe zaburzenia
            cells[i][j].height += heightDist(gen) * 0.1;
        }
    }
}

void Grid::setSources(int count) {
    std::mt19937 gen(std::random_device{}());
    // ród³a w górnych partiach (œrodek mapy wg naszej generacji)
    std::uniform_int_distribution<> dist(N / 3, 2 * N / 3);

    for (int k = 0; k < count; ++k) {
        int r = dist(gen);
        int c = dist(gen);
        cells[r][c].is_source = true;
    }
}

void Grid::printWaterMap() const {
    std::cout << "\n--- Mapa Wody ---\n";
    for (const auto& row : cells) {
        for (const auto& cell : row) {
            if (cell.water < 0.1) std::cout << ". ";
            else if (cell.water < 10) std::cout << "~ ";
            else std::cout << "# ";
        }
        std::cout << "\n";
    }
}

void Grid::printTopRisks(int limit) const {
    struct RiskInfo { int r, c; double risk; double water; };
    std::vector<RiskInfo> risks;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (cells[i][j].flood_risk > 0)
                risks.push_back({ i, j, cells[i][j].flood_risk, cells[i][j].water });
        }
    }

    std::sort(risks.begin(), risks.end(), [](const auto& a, const auto& b) {
        return a.risk > b.risk;
        });

    std::cout << "\n--- TOP " << limit << " Ryzyka Powodzi ---\n";
    for (int i = 0; i < std::min((int)risks.size(), limit); ++i) {
        std::cout << "Komorka [" << risks[i].r << "," << risks[i].c
            << "] Ryzyko: " << std::fixed << std::setprecision(2) << risks[i].risk
            << " (Woda: " << risks[i].water << ")\n";
    }
}

void Grid::saveToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    // Format: rzad, kolumna, wysokosc, woda, ryzyko
    file << "x,y,height,water,risk\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            file << j << "," << i << ","
                << cells[i][j].height << ","
                << cells[i][j].water << ","
                << cells[i][j].flood_risk << "\n";
        }
    }
    file.close();
    std::cout << "Dane zapisano do pliku: " << filename << std::endl;
}
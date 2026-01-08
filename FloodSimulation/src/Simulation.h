#pragma once
#include "Grid.h"
#include <vector>
#include <thread>
#include <barrier>

class Simulation {
public:
    Simulation(int gridSize, int iterations);
    void run();
    void printResults();

private:
    void workerThread(int rowIdx, std::barrier<>& syncPoint);
    void calculateFlow(int row, int col);

    int N;
    int iterations;
    Grid grid;

    double snowLevel = 80.0;
    double rainProbability = 0.1;
};
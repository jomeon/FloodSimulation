#pragma once

struct Cell {
    double height = 0.0;        // Wysokoœæ terenu
    double water = 0.0;         // Aktualna iloœæ wody
    double flow_out = 0.0;      // Ile wody wyp³ywa w tej iteracji
    double capacity = 15.0;     // Maksymalna pojemnoœæ koryta
    double flood_risk = 0.0;    // Wspó³czynnik 0.0 - 1.0

    bool is_source = false;

    // Pomocnicza metoda do obliczania ryzyka
    void updateRisk() {
        if (water > capacity) {
            flood_risk = (water - capacity) / capacity;
            if (flood_risk > 1.0) flood_risk = 1.0;
        }
        else {
            flood_risk = 0.0;
        }
    }
};
# -*- coding: utf-8 -*-
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

csv_file = "wyniki.csv"

if not os.path.exists(csv_file):
    print(f"BLAD: Nie znaleziono pliku {csv_file}")
    print("Upewnij sie, ze uruchamiasz skrypt w tym samym folderze, w ktorym jest plik CSV.")
    print(f"Obecny katalog roboczy to: {os.getcwd()}")
    exit()


try:
    df = pd.read_csv(csv_file)
except Exception as e:
    print(f"Blad podczas odczytu CSV: {e}")
    exit()

try:
    water_map = df.pivot(index="y", columns="x", values="water")
    risk_map = df.pivot(index="y", columns="x", values="risk")
    height_map = df.pivot(index="y", columns="x", values="height")
except KeyError as e:
    print(f"Blad danych: Brakuje kolumny {e} w pliku CSV. Sprawdz czy C++ dobrze zapisal naglowki.")
    exit()

fig, axes = plt.subplots(1, 3, figsize=(18, 5))

# Mapa Terenu
sns.heatmap(height_map, ax=axes[0], cmap="terrain", cbar_kws={'label': 'Wysokosc'})
axes[0].set_title("Uksztaltowanie Terenu")
axes[0].invert_yaxis()

# Mapa Wody
sns.heatmap(water_map, ax=axes[1], cmap="Blues", cbar_kws={'label': 'Ilosc wody'})
axes[1].set_title("Rozmieszczenie Wody")
axes[1].invert_yaxis()

# Mapa Zagro¿enia
sns.heatmap(risk_map, ax=axes[2], cmap="Reds", vmin=0, vmax=1, cbar_kws={'label': 'Ryzyko (0-1)'})
axes[2].set_title("Ryzyko Powodziowe")
axes[2].invert_yaxis()

plt.tight_layout()
print("Wyswietlanie wykresu...")
plt.show()
import pandas as pd
import matplotlib.pyplot as plt
import os
import numpy as np

# Ścieżki do plików CSV
file_path_1 = 'P:/STM32/INZ_testy_napedow_F746/INZ_dane_i_wykresy/2_pióra_skokowy_14Hz.csv'
file_path_2 = 'P:/STM32/INZ_testy_napedow_F746/INZ_dane_i_wykresy/2_pióra_skokowy_bez_filtracji.csv'
input_signal_path = 'P:/STM32/INZ_testy_napedow_F746/SYGNAL_SKOKOWY2.csv'

# Wczytanie danych z pierwszego pliku
data1 = pd.read_csv(file_path_1)
time1 = data1['Time']
speed1 = data1['Speed']

# Wczytanie danych z drugiego pliku
data2 = pd.read_csv(file_path_2)
time2 = data2['Time']
speed2 = data2['Speed']

# Wczytanie pliku sygnału skokowego
input_data = pd.read_csv(input_signal_path, header=None)

# Tworzenie osi czasu (próbki co 0.2s)
input_time = [i * 0.2 for i in range(len(input_data))]
input_signal = (input_data.iloc[:, 0] / 254) * 100  # Normalizacja do zakresu 0-100

plt.figure(figsize=(10, 6))

# Tworzenie głównej osi (dla prędkości kątowej)
fig, ax1 = plt.subplots(figsize=(10, 6))

# Druga oś Y dla sygnału skokowego
ax2 = ax1.twinx()
ax2.step(input_time, input_signal, label=r"$u_{PWM}$(t)", linestyle='-.', color='red', where='post')
ax2.set_ylabel('Wypełnienie PWM [%]')

# Wykresy prędkości kątowych (lewa oś)
ax1.plot(time2, speed2, label=r"$\omega$(t)", color='orange', linewidth=0.7)
ax1.plot(time1, speed1, label=r"$\omega_f$(t)", color='blue', linewidth=0.7)

ax1.set_xlabel('Czas [s]')
ax1.set_ylabel('Obroty na sekundę [Hz]')

lines1, labels1 = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()
ax1.legend(lines1 + lines2, labels1 + labels2, loc='best')  # Łączy legendy w jedno miejsce

ax1.grid(True, which='both', linestyle='--', linewidth=0.5)  # "both" = główna i pomocnicza siatka
ax2.grid(False)  # Wyłącz siatkę na drugiej osi (jeśli nie jest potrzebna)

ax1.set_xticks(np.arange(0, 3.6, 0.2))  # Oś X: podziałki co 0.1 sekundy
ax1.set_yticks(np.arange(0, 95, 5))  # Oś Y1 (prędkość): podziałki co 5 Hz
ax2.set_yticks(np.arange(0, 105, 10))  # Oś Y2 (PWM): podziałki co 10%

ax1.set_xlim(ax1.set_xlim(0, 3.5))  # Wykres kończy się na 3.5 sekundy

# Zapis wykresu do pliku PDF
angles_pdf_path = os.path.join(os.path.dirname(file_path_1), 'SYGNAL_SKOKOWY_filtrowany_niefiltrowany_wymuszenie.pdf')
plt.savefig(angles_pdf_path)
plt.show()


print("Wykres zapisano jako plik PDF:")
print(f"- Ścieżka: {angles_pdf_path}")

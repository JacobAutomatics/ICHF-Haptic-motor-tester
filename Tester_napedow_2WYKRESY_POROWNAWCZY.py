import pandas as pd
import matplotlib.pyplot as plt
import os
import numpy as np

# Ścieżki do plików CSV
file_path_1 = 'P:/STM32/INZ_testy_napedow_F746/INZ_dane_i_wykresy/2_pióra_skokowy_14Hz.csv'
file_path_2 = 'P:/STM32/INZ_testy_napedow_F746/INZ_dane_i_wykresy/4_pióra_skokowy_14Hz.csv'

# Wczytanie danych z pierwszego pliku
data1 = pd.read_csv(file_path_1)
time1 = data1['Time']
speed1 = data1['Speed']

# Wczytanie danych z drugiego pliku
data2 = pd.read_csv(file_path_2)
time2 = data2['Time']
speed2 = data2['Speed']

# Znalezienie zakresu czasu dla pierwszego pliku
start_time1 = time1[speed1.first_valid_index()]
end_time1 = time1[speed1.last_valid_index()]

# Znalezienie zakresu czasu dla drugiego pliku
start_time2 = time2[speed2.first_valid_index()]
end_time2 = time2[speed2.last_valid_index()]

plt.figure(figsize=(10, 6))
plt.xticks(np.arange(0, 3.6, 0.2))  # Podziałki na osi X
plt.yticks(np.arange(0, 90, 5))   # Podziałki na osi Y
plt.plot(time2, speed2, label=r"$\omega_f^{4p}$(t)", color='green', linewidth=0.8)
plt.plot(time1, speed1, label=r"$\omega_f^{2p}$(t)", color='blue', linewidth=0.8)

plt.xlabel('Czas [s]')
plt.ylabel('Obroty na sekundę [Hz]')
# plt.xticks(fontsize=16)  # Wielkość wartości na osi X
# plt.yticks(fontsize=16)  # Wielkość wartości na osi Y
plt.legend(loc='best')

plt.grid(True, which='both', linestyle='--', linewidth=0.5)  # "both" = siatka dla osi X i Y

# Automatyczne ustawienie zakresu osi czasu (uwzględniając oba pliki)
global_start_time = min(start_time1, start_time2)
global_end_time = max(end_time1, end_time2)
# plt.xlim(global_start_time, global_end_time)
plt.xlim(0, 3.5)

# Zapis wykresu do pliku PDF
angles_pdf_path = os.path.join(os.path.dirname(file_path_1), '2chartSpeed_compare.pdf')
plt.savefig(angles_pdf_path)
plt.show()

print("Wykres zapisano jako plik PDF:")
print(f"- Ścieżka: {angles_pdf_path}")

import pandas as pd
import matplotlib.pyplot as plt
import os

# Ścieżka do pliku CSV
file_path = 'P:/STM32/INZ_testy_napedow_F746/Test_napeduX3.csv'

# Wczytanie danych
data = pd.read_csv(file_path)

# Sprawdzenie struktury danych (opcjonalne)
print(data.head())

# Czas (zakładam, że czas jest w pierwszej kolumnie)
time = data['Time']

# Wykresy prędkości kątowych (zakładam, że kolejne kolumny to różne prędkości)
speed1 = data['Speed1']  # Prędkość dla pierwszego wykresu
speed2 = data['Speed2']  # Prędkość dla drugiego wykresu
speed3 = data['Speed3']  # Prędkość dla trzeciego wykresu

start_time1 = time[speed1.first_valid_index()]  # Pierwszy czas, gdy jest jakakolwiek wartość
end_time1 = time[speed1.last_valid_index()]    # Ostatni czas, gdy jest jakakolwiek wartość
start_time2 = time[speed2.first_valid_index()]  # Pierwszy czas, gdy jest jakakolwiek wartość
end_time2 = time[speed2.last_valid_index()]    # Ostatni czas, gdy jest jakakolwiek wartość
start_time3 = time[speed3.first_valid_index()]  # Pierwszy czas, gdy jest jakakolwiek wartość
end_time3 = time[speed3.last_valid_index()]    # Ostatni czas, gdy jest jakakolwiek wartość

# Tworzenie wykresu
plt.figure(figsize=(80, 48))

# Rysowanie pierwszego wykresu
plt.plot(time, speed1, label='Wykres 1: Bez filtracji')

# Nakładanie drugiego wykresu
plt.plot(time, speed2, label='Wykres 2: Filtr nr 2')

# Nakładanie trzeciego wykresu
plt.plot(time, speed3, label='Wykres 3: Filtr nr 3')

# Ustawienia osi i legendy
plt.xlabel('Czas [s]')
plt.ylabel('Obroty na sekundę [Hz]')
plt.legend()
plt.grid(True)

global_start_time = min(start_time1, start_time2, start_time3)
global_end_time = max(end_time1, end_time2, end_time3)
plt.xlim(global_start_time, global_end_time)


# Zapis do pliku PDF
output_path = os.path.join(os.path.dirname(file_path), '3ChartSpeeds.pdf')
plt.savefig(output_path)
plt.show()

print("Wykres zapisano jako plik PDF:")
print(f"- Ścieżka: {output_path}")

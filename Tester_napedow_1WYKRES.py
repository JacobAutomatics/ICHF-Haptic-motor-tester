import pandas as pd
import matplotlib.pyplot as plt
import os

file_path = 'P:/STM32/INZ_testy_napedow_F746/Test_napedu.csv'
# file_path = 'P:/STM32/INZ_testy_napedow_F746/INZ_dane_i_wykresy/Test_napedu.csv'

data = pd.read_csv(file_path)
print(data.head())

time = data['Time']

speed = data['Speed']

# Znalezienie zakresu czasu, w którym są dane wykresu
start_time = time[speed.first_valid_index()]  # Pierwszy czas, gdy jest jakakolwiek wartość
end_time = time[speed.last_valid_index()]    # Ostatni czas, gdy jest jakakolwiek wartość

plt.figure(figsize=(10, 6))
plt.plot(time, speed, label='prędkość kątowa', linewidth=1)
plt.xlabel('Czas [s]')
plt.ylabel('obroty na sekundę [Hz]')
plt.legend()
plt.grid(True)

plt.xlim(start_time, end_time) # Ograniczenie zakresu wyświetlania osi czasu

angles_pdf_path = os.path.join(os.path.dirname(file_path), '1chartSpeed.pdf')
plt.savefig(angles_pdf_path)
plt.show()

print("Wykres zapisano jako plik PDF:")
print(f"- Ścieżka: {angles_pdf_path}")

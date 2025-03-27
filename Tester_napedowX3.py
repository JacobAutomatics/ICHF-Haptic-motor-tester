import csv
import serial
import threading
import time
import os

# Ustawienia portu szeregowego
port = "COM4"  # Zmień na odpowiedni numer portu
baudrate = 230400

# Tworzenie instancji portu szeregowego
ser = serial.Serial(port, baudrate)

shutDownFlag = False  # Flaga kończąca wysyłanie ramek
first_sample_time_set = False  # Flaga wskazujaca, czy zapisano czas pierwszej probki
start_time = None  # Czas poczatku pierwszej probki

# Ścieżka do pliku CSV
csv_file_path = os.path.join(os.path.dirname(__file__), "Test_napeduX3.csv")

# Funkcja wysyłająca dane z pliku CSV
def send_data_from_csv():
    global shutDownFlag

    # Wczytanie danych z pliku CSV
    csv_file_path_source = r"P:\STM32\INZ_testy_napedow_F746\SYGNAL_DYNAMICZNY.csv"  # Ścieżka do pliku CSV
    data = []

    try:
        with open(csv_file_path_source, mode="r") as file:
            csvreader = csv.reader(file)
            for row in csvreader:
                try:
                    data.append(int(row[0]))
                except ValueError:
                    continue  # Pomijanie błędnych wierszy
    except FileNotFoundError:
        print("Nie znaleziono pliku CSV.")
        return

    # Obsługa startu pomiarów
    print("Aby rozpocząć pomiar, wpisz 's' i wciśnij Enter:")
    user_input = input().strip()

    if user_input != 's':
        print("Błąd: Niepoprawna komenda. Użyj 's' do rozpoczęcia.")
        return

    print("Rozpoczęcie wysyłania danych...")

    for value in data:
        if value >= 255:
            print("Koniec testu")
            shutDownFlag = True
            break

        # Tworzenie ramki danych
        frame = bytes([0b00000001, 0b00000000, value, 0b11111111])

        # Wysyłanie ramki przez port szeregowy
        ser.write(frame)

        # Skok czasowy Tp
        time.sleep(0.2)

    print("Wysyłanie danych zakończone.")

# Funkcja odbierająca dane z portu szeregowego
def receive_data():
    global shutDownFlag
    global first_sample_time_set
    global start_time

    received_data = []
    byte_count = 0
    bytes_in_frame = 6  # Zmienione na 6 bajtów

    # Otwieranie pliku CSV do zapisu
    with open(csv_file_path, mode="w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(["Speed1", "Speed2", "Speed3", "Time"])  # Nagłówki kolumn

        while not shutDownFlag:
            if ser.in_waiting > 0:
                byte = ser.read(1)
                int_byte = int.from_bytes(byte, "big")

                if int_byte != 0b11111111 and byte_count < bytes_in_frame:
                    received_data.append(int_byte)
                    byte_count += 1
                else:
                    if int_byte == 0b11111111 and byte_count == bytes_in_frame:
                        ASinput_0 = received_data[0]
                        ASinput_1 = received_data[1]
                        ASinput_2 = received_data[2]
                        ASinput_3 = received_data[3]
                        ASinput_4 = received_data[4]
                        ASinput_5 = received_data[5]

                        # Składanie zmiennych speed
                        speed1 = round((((ASinput_0 << 3) & 0b1111111000) | (ASinput_1 & 0b111)) / 11.37, 3)
                        speed2 = round((((ASinput_2 << 3) & 0b1111111000) | (ASinput_3 & 0b111)) / 11.37, 3)
                        speed3 = round((((ASinput_4 << 3) & 0b1111111000) | (ASinput_5 & 0b111)) / 11.37, 3)

                        # Ustaw czas pierwszej próbki
                        if not first_sample_time_set:
                            start_time = time.time()
                            first_sample_time_set = True

                        # Oblicz czas od początku pierwszej próbki
                        elapsed_time = time.time() - start_time

                        # Zapis do pliku CSV
                        csvwriter.writerow([speed1, speed2, speed3, elapsed_time])

                        # Wyświetlanie wartości zmiennych speed
                        print(f"Odebrano speed1: {speed1}, speed2: {speed2}, speed3: {speed3}")

                        # Resetowanie licznika i danych
                        byte_count = 0
                        received_data.clear()
                    else:
                        # Resetowanie licznika i danych w przypadku błędu
                        byte_count = 0
                        received_data.clear()

# Funkcja monitorująca działanie programu
def monitor_program():
    global shutDownFlag

    while not shutDownFlag:
        time.sleep(0.5)

    print("Monitorowanie zakończone.")

# Konfiguracja wątków
try:
    thread_send_data = threading.Thread(target=send_data_from_csv)
    thread_receive_data = threading.Thread(target=receive_data)
    thread_monitor = threading.Thread(target=monitor_program)

    thread_send_data.start()
    thread_receive_data.start()
    thread_monitor.start()

    thread_send_data.join()
    thread_receive_data.join()
    thread_monitor.join()

finally:
    shutDownFlag = True
    ser.close()
    print(f"Program zakończony. Dane zapisano w pliku: {csv_file_path}")

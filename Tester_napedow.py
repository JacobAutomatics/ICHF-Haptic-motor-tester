import csv
import serial
import threading
import time
import os

# Ustawienia portu szeregowego
port = "COM3"
baudrate = 500000

ser = serial.Serial(port, baudrate)

shutDownFlag = False  # Flaga kończąca wysyłanie ramek
first_sample_time_set = False  # Flaga wskazujaca, czy zapisano czas pierwszej probki
start_time = None  # Czas poczatku pierwszej probki

# Ścieżka do pliku z testem
csv_file_path = os.path.join(os.path.dirname(__file__), "Test_napedu.csv")

# Wątek wysyłający dane testowe
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

        frame = bytes([0b00000001, value, 0b00000000, 0b11111111])
        ser.write(frame)

        time.sleep(0.2)

    print("Wysyłanie danych zakończone.")




# Wątek odbierający dane z mikrokontrolera
def receive_data():
    global shutDownFlag
    global first_sample_time_set
    global start_time

    motorL = 0
    motorR = 0
    
    angleX = 0
    angleY = 0
    accelX = 0
    accelY = 0
    accelZ = 0
    buttonsA = 0
    buttonsB = 0
    joystickX = 0
    joystickY = 0

    receivedBytes = []
    byte_count = 0
    bytesInFrame = 18

    # Otwieranie pliku CSV do zapisu
    with open(csv_file_path, mode="w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(["Speed", "Time"])  # Nagłówki kolumn

        while not shutDownFlag:
            if ser.in_waiting > 0:
                byte = ser.read(1)
                int_byte = int.from_bytes(byte, "big")

                if int_byte != 0b11111111 and byte_count < bytesInFrame:
                    receivedBytes.append(int_byte)
                    byte_count += 1
                else:
                    if int_byte == 0b11111111 and byte_count == bytesInFrame:

                        for i in range(bytesInFrame):
                            if receivedBytes[i] != 0b11111111 and i == (bytesInFrame - 1):
                                angleX = ((receivedBytes[0] << 6) & 0b11111110000000) | (receivedBytes[1] & 0b1111111)
                                angleY = ((receivedBytes[2] << 6) & 0b11111110000000) | (receivedBytes[3] & 0b1111111)
                                motorL = ((receivedBytes[4] << 6) & 0b11111110000000) | (receivedBytes[5] & 0b1111111)
                                motorR = ((receivedBytes[6] << 6) & 0b11111110000000) | (receivedBytes[7] & 0b1111111)
                                buttonsA = (receivedBytes[8] & 0b11111011) | (receivedBytes[9] & 0b100)
                                buttonsB = receivedBytes[9] & 0b1111011
                                accelX = ((receivedBytes[10] << 6) & 0b11111110000000) | (receivedBytes[11] & 0b1111111)
                                accelY = ((receivedBytes[12] << 6) & 0b11111110000000) | (receivedBytes[13] & 0b1111111)
                                accelZ = ((receivedBytes[14] << 6) & 0b11111110000000) | (receivedBytes[15] & 0b1111111)
                                joystickX = receivedBytes[16]
                                joystickY = receivedBytes[17]

                                angleX = (angleX - 8192) / 45.50556
                                angleY = (angleY - 8192) / 45.50556
                                motorL /= 182.03333
                                motorR /= 182.03333
                                accelX = (accelX - 8192) / 2047.75
                                accelY = (accelY - 8192) / 2047.75
                                accelZ = (accelZ - 8192) / 2047.75
                                joystickX = round((joystickX + 0.5) / 0.9961)
                                joystickY = round((joystickY + 0.5) / 0.9961)


                                # Ustaw czas pierwszej próbki
                                if not first_sample_time_set:
                                    start_time = time.time()
                                    first_sample_time_set = True

                                # Oblicz czas od początku pierwszej próbki
                                elapsed_time = time.time() - start_time

                        csvwriter.writerow([motorL, elapsed_time])

                        print(f"Odebrano prędkość: {motorL} Hz")

                        byte_count = 0
                        receivedBytes.clear()
                    else:
                        byte_count = 0
                        receivedBytes.clear()

# Wątek nadzorujący działanie
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

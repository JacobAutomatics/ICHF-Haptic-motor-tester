/*
 * AS5600.c
 *
 *  Created on: Dec 21, 2025
 *      Author: Jakub Izydorczyk
 */

#include "main.h"
#include "i2c.h"
#include "AS5600.h"


#define AS5600_ADDR (0x36 << 1) // adres w magistrali I2C (zapis)


uint8_t AS5600_Init(I2C_HandleTypeDef *I2Cx, AS5600_t *Datastruct) {
    uint8_t confData[2];
    uint8_t testRead;

    // Sprawdzenie komunikacji przez odczyt dowolnego rejestru (np. CONF - 0x07)
    if (HAL_I2C_Mem_Read(I2Cx, AS5600_ADDR, 0x07, I2C_MEMADD_SIZE_8BIT, &testRead, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 1; // Błąd komunikacji
    }

    // Wyłączenie filtrów (ustawienie FFT = 0 i SLOW Filter OFF)
    confData[0] = 0x00; // MSB: FFT = 0 (brak filtra), SLOW Filter OFF
    confData[1] = 0x00; // LSB: Domyślne wartości
    if (HAL_I2C_Mem_Write(I2Cx, AS5600_ADDR, 0x07, I2C_MEMADD_SIZE_8BIT, confData, 2, HAL_MAX_DELAY) != HAL_OK) {
        return 2; // Błąd zapisu do rejestru CONF
    }

    // Opcjonalnie: Ustawienie pozycji zerowej (ZPOS) i maksymalnej (MPOS)
    uint8_t zeroPosition[2] = {0x00, 0x00}; // ZPOS = 0 (0°)
    if (HAL_I2C_Mem_Write(I2Cx, AS5600_ADDR, 0x01, I2C_MEMADD_SIZE_8BIT, zeroPosition, 2, HAL_MAX_DELAY) != HAL_OK) {
        return 3; // Błąd ustawiania ZPOS
    }

    uint8_t maxPosition[2] = {0x0F, 0xFF}; // MPOS = 0x0FFF (360°)
    if (HAL_I2C_Mem_Write(I2Cx, AS5600_ADDR, 0x03, I2C_MEMADD_SIZE_8BIT, maxPosition, 2, HAL_MAX_DELAY) != HAL_OK) {
        return 4; // Błąd ustawiania MPOS
    }

    // Inicjalizacja zakończona sukcesem
    return 0;
}


void AS5600_ReadAngle(I2C_HandleTypeDef *I2Cx ,AS5600_t *Datastruct) {
    uint8_t angleData[2] = {0};
    uint16_t angle = 0;

    // Odczyt 2 bajtów z rejestru ANGLE (0x0E i 0x0F)
    if (HAL_I2C_Mem_Read(I2Cx, AS5600_ADDR, 0x0E, I2C_MEMADD_SIZE_8BIT, angleData, 2, HAL_MAX_DELAY) == HAL_OK) {
        angle = (angleData[0] << 8) | angleData[1];
        angle &= 0x0FFF; // Maskowanie do 12 bitów
    }
    Datastruct->position = angle;
}


void AS5600_UpdateTurns(AS5600_t *Datastruct) {

    // Obliczenie różnicy pozycji
    int16_t deltaPosition = Datastruct->position - Datastruct->lastPosition;

    // Obsługa przejścia przez 360°/0
    if (deltaPosition < -2048) { // Przejście z 360° na 0°
        Datastruct->turns++;     // Zwiększ licznik obrotów
    }

    // Aktualizacja pełnej pozycji i ostatniej pozycji
    Datastruct->lastPosition = Datastruct->position;
}

void AS5600_CalculateSpeed(I2C_HandleTypeDef *I2Cx, AS5600_t *Datastruct) {

    int32_t currentFullPosition = Datastruct->turns * 4096 + Datastruct->position; // Pełna pozycja

    // Obliczenie różnicy czasu
    uint32_t currentTime = HAL_GetTick(); // Aktualny czas w ms
    float deltaTime = (currentTime - Datastruct->lastTime) / 1000.0f; // ms -> s

    if (deltaTime > 0) {
        // Obliczenie prędkości (obroty na sekundę)
        int32_t deltaPosition = currentFullPosition - Datastruct->lastFullPosition;
        Datastruct->speed = deltaPosition / (4096.0f * deltaTime);
    } else {
        Datastruct->speed = 0; // W przypadku zbyt małej różnicy czasu
    }

    // Aktualizacja stanu
    Datastruct->lastFullPosition = currentFullPosition;
    Datastruct->lastTime = currentTime;
}



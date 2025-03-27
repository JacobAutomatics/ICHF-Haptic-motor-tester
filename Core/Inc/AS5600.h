/*
 * AS5600.h
 *
 *  Created on: Dec 15, 2025
 *      Author: Jakub Izydorczyk
 */

#ifndef INC_AS5600_H_
#define INC_AS5600_H_

#include <stdint.h>
#include "i2c.h"

#endif /* INC_AS5600_H_ */


// AS5600 structure
typedef struct
{

    uint16_t position; // 12-bitowa głębia
    float speed; // obroty na sekunde

    // Potrzebne do obsługi kontroli przejścia 360->0
    int32_t turns; // liczba pełnych obrotów
    uint16_t lastPosition; // ostatnia odczytana pozycja

    // Potrzebne do obliczania prędkości kątowej
    int32_t lastFullPosition; // Poprzednia pełna pozycja
    uint32_t lastTime;        // Poprzedni czas w ms

} AS5600_t;

uint8_t AS5600_Init(I2C_HandleTypeDef *I2Cx, AS5600_t *Datastruct);

void AS5600_ReadAngle(I2C_HandleTypeDef *I2Cx, AS5600_t *Datastruct);

void AS5600_UpdateTurns(AS5600_t *Datastruct);

void AS5600_CalculateSpeed(I2C_HandleTypeDef *I2Cx, AS5600_t *Datastruct);

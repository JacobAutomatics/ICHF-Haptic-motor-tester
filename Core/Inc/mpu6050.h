#ifndef INC_GY521_H_
#define INC_GY521_H_

#endif /* INC_GY521_H_ */

#include <stdint.h>
#include "i2c.h"

// MPU6050 structure
typedef struct
{

    int16_t Accel_X_RAW;	// wartości pobrane bezpośśrednio z akcelerometru
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;

    float Ax;	// wartosci przeskalowane na przyspieszenie
    float Ay;
    float Az;

    int16_t Gyro_X_RAW;	// wartości pobrane bezpośrednio z zyroskopu
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;

    float Gx;	// wartosci przeskalowane na kat
    float Gy;
    float Gz;

    float Temperature;

    float roll[2];	// kat obliczony w osi x oraz y (do filtru kalmana)
    float pitch[2];

    float KalmanAngleX; // OUTPUT z filtra kalmana
    float KalmanAngleY;

    uint16_t angleXscaled;	// wartosci OUTPUT przeskalowane na typ uint16_t
    uint16_t angleYscaled;

    unsigned int flag;  // TYLKO DO TESTOW

} MPU6050_t;

// Kalman structure
typedef struct
{
	float Q_angle;
	float Q_bias;
	float R_measure;
	float angle;
	float b_k;
	float P_k[2][2];
} Kalman_t;

uint8_t MPU6050_Init(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

void MPU6050_Read_All180(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

void MPU6050_Angles_Coding_Process(MPU6050_t sensorStruct, uint8_t output[3]);

void MPU6050_Accel_to_Byte(MPU6050_t sensorStruct, uint8_t output[3]);

float MPU_6050_Kalman_getAngle(Kalman_t *Kalman, float newAngle, float newRate, float dt);

//float MPU6050_Get_Temperature(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

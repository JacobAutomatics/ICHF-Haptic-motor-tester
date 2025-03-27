/*
 * Butterworth_I_eulerF.c
 *
 *  Created on: Jan 22, 2024
 *      Author: Jakub Izydorczyk
 */

#include "main.h"
#include "math.h"
#include "Butterworth_I_eulerF.h"

void Butterworth_I_eulerF_Init(Butterwoth_I_eulerF_t *Filter, float freq, float Tp) {
	// freq ma być w hercach, Tp w milisekundach
	Filter->alpha = 2 * M_PI * freq * (Tp / 1000);
	Filter->u1 = 0;
	Filter->y = 0;
	Filter->y1 = 0;
}

void Butterworth_I_eulerF_Update(Butterwoth_I_eulerF_t *Filter, float input) {
	Filter->y = (Filter->u1 * Filter->alpha) - (Filter->y1 * Filter->alpha) + Filter->y1;
	Filter->y1 = Filter->y;
	Filter->u1 = input;
}

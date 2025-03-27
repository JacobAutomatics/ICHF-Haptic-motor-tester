/*
 * Butterworth_I_tustin.c
 *
 *  Created on: Jan 26, 2025
 *      Author: User
 */


#include "main.h"
#include "math.h"
#include "Butterworth_I_tustin.h"

void Butterworth_I_tustin_Init(Butterwoth_I_tustin_t *Filter, float freq, float Tp) {
	Filter->alpha = 2 * M_PI * freq * (Tp / 1000);

	Filter->ucoef = Filter->alpha / (2 + Filter->alpha);
	Filter->u1coef = Filter->ucoef;
	Filter->y1coef = (2 - Filter->alpha) / (2 + Filter->alpha);

	Filter->u1 = 0;
	Filter->y = 0;
	Filter->y1 = 0;
}

void Butterworth_I_tustin_Update(Butterwoth_I_tustin_t *Filter, float input) {
	Filter->y = (input * Filter->ucoef) + (Filter->u1 * Filter->u1coef) + (Filter->y1 * Filter->y1coef);
	Filter->y1 = Filter->y;
	Filter->u1 = input;
}

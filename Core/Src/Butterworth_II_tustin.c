/*
 * Butterworth_II_tustin.c
 *
 *  Created on: Jan 22, 2024
 *      Author: Jakub Izydorczyk
 */

#include "main.h"
#include "math.h"
#include "Butterworth_II_tustin.h"

void Butterworth_II_tustin_Init(Butterwoth_II_tustin_t *Filter, float freq, float Tp) {
	// freq jest w Hz
	// Tp jest w milisekundach
	Filter->alpha = 2 * M_PI * freq * (Tp / 1000);

	Filter->u2coef = (Filter->alpha * Filter->alpha) / (2 * sqrt(2) * Filter->alpha + 4 + Filter->alpha * Filter->alpha);
	Filter->u1coef = Filter->u2coef * 2;
	Filter->ucoef = Filter->u2coef;
	Filter->y2coef = (2 * sqrt(2) * Filter->alpha - Filter->alpha * Filter->alpha - 4) / (2 * sqrt(2) * Filter->alpha + 4 + Filter->alpha * Filter->alpha);
	Filter->y1coef = (8 - 2 * Filter->alpha * Filter->alpha) / (2 * sqrt(2) * Filter->alpha + 4 + Filter->alpha * Filter->alpha);

	Filter->u1 = 0;
	Filter->u2 = 0;
	Filter->y = 0;
	Filter->y1 = 0;
	Filter->y2 = 0;
}

void Butterworth_II_tustin_Update(Butterwoth_II_tustin_t *Filter, float input) {
	Filter->y = (Filter->u2 * Filter->u2coef) + (Filter->u1 * Filter->u1coef) + (input * Filter->ucoef) + (Filter->y2 * Filter->y2coef) + (Filter->y1 * Filter->y1coef);
	Filter->y2 = Filter->y1;
	Filter->y1 = Filter->y;
	Filter->u2 = Filter->u1;
	Filter->u1 = input;
}

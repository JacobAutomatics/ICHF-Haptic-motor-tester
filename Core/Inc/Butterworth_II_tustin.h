/*
 * Butterworth_II_tustin.h
 *
 *  Created on: Jan 24, 2025
 *      Author: User
 */

#ifndef INC_BUTTERWORTH_II_TUSTIN_H_
#define INC_BUTTERWORTH_II_TUSTIN_H_



#endif /* INC_BUTTERWORTH_II_TUSTIN_H_ */

typedef struct {
	float alpha;
	float u2coef;
	float u1coef;
	float ucoef;
	float y2coef;
	float y1coef;

	float u2;
	float u1;
	float y2;
	float y1;
	float y;
} Butterwoth_II_tustin_t;

void Butterworth_II_tustin_Init(Butterwoth_II_tustin_t *Filter, float freq, float Tp);

void Butterworth_II_tustin_Update(Butterwoth_II_tustin_t *Filter, float input);



/*
 * Butterworth_I_tustin.h
 *
 *  Created on: Jan 26, 2025
 *      Author: User
 */

#ifndef INC_BUTTERWORTH_I_TUSTIN_H_
#define INC_BUTTERWORTH_I_TUSTIN_H_



#endif /* INC_BUTTERWORTH_I_TUSTIN_H_ */

typedef struct {
	float ucoef;
	float u1coef;
	float y1coef;

	float alpha;
	float u1;
	float y1;
	float y;
} Butterwoth_I_tustin_t;

void Butterworth_I_tustin_Init(Butterwoth_I_tustin_t *Filter, float freq, float Tp);

void Butterworth_I_tustin_Update(Butterwoth_I_tustin_t *Filter, float input);

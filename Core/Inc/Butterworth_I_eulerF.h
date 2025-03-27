/*
 * Butterworth_I_eulerF.h
 *
 *  Created on: Jan 24, 2025
 *      Author: User
 */

#ifndef INC_BUTTERWORTH_I_EULERF_H_
#define INC_BUTTERWORTH_I_EULERF_H_



#endif /* INC_BUTTERWORTH_I_EULERF_H_ */

typedef struct {
	float alpha;
	float u1;
	float y1;
	float y;
} Butterwoth_I_eulerF_t;

void Butterworth_I_eulerF_Init(Butterwoth_I_eulerF_t *Filter, float freq, float Tp);

void Butterworth_I_eulerF_Update(Butterwoth_I_eulerF_t *Filter, float input);

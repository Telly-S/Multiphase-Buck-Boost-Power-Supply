#ifndef _PWM_PID_H
#define _PWM_PID_H

#include "buck-boost.h"

typedef enum{p1, p2} phasex_t;

void pid_amend_u(uint16_t p, uint16_t i, uint16_t d, uint8_t mode);

//int phase_pid_u(float practice, float target, phasex_t phasex);
int phase_pid_u(float practice, float target, phasex_t phasex, RunMode_t mode, bool isFirstIn);
int phase_pid_i(float practice, float target, phasex_t phasex);

#endif

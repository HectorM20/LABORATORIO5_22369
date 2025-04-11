
#ifndef TIMER2PWM_H_
#define TIMER2PWM_H_
#include <avr/io.h>
#include <stdint.h>

//Prototipos para la generar PWM manual con Timer2
void initTimer2ManualPWM(uint16_t prescaler, uint8_t period);

void setTimer2Duty(uint8_t duty);

#endif /* TIMER2PWM_H_ */
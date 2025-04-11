#include "timer2PWM.h"
#include <avr/interrupt.h>

//Variables internas para la generación del PWM
static volatile uint8_t timer2_period = 100;			//Período del PWM
static volatile uint8_t timer2_duty   = 0;				//Duty deseado
static volatile uint8_t timer2_counter = 0;				//Contador de software

void initTimer2ManualPWM(uint16_t prescaler, uint8_t period)
{
	timer2_period = period;
	timer2_counter = 0;
	
	//Configurar el pin OC2A en PB3
	DDRB |= (1 << PB3);
	
	//Configurar Timer2 en modo CTC:
	TCCR2A = (1 << WGM21);
	
	//Configurar prescaler para Timer2.
	TCCR2B &= ~((1 << CS22)|(1 << CS21)|(1 << CS20));
	switch(prescaler)
	{
		case 1:
		TCCR2B |= (1 << CS20);
		break;
		case 8:
		TCCR2B |= (1 << CS21);
		break;
		case 32:
		TCCR2B |= (1 << CS21) | (1 << CS20);
		break;
		case 64:
		TCCR2B |= (1 << CS22);
		break;
		case 128:
		TCCR2B |= (1 << CS22) | (1 << CS20);
		break;
		case 256:
		TCCR2B |= (1 << CS22) | (1 << CS21);
		break;
		case 1024:
		TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
		break;
		default:
		break;
	}
	
	//Valor menor = interrupciones más frecuente
	OCR2A = 4;
	
	//Habilitar la interrupción de comparación de Timer2 (canal A).
	TIMSK2 |= (1 << OCIE2A);
}

void setTimer2Duty(uint8_t duty)
{
	if(duty > timer2_period)
	duty = timer2_period;
	timer2_duty = duty;
}

//ISR para el Compare Match de Timer2 (OCIE2A)
ISR(TIMER2_COMPA_vect)
{
	//Incrementamos el contador de software.
	timer2_counter++;
	if(timer2_counter >= timer2_period)
	timer2_counter = 0;			//Se reinicia a 0, es decir se completa un ciclo PWM
	
	//Generamos el PWM manual comparando el contador con el duty deseado.
	if(timer2_counter < timer2_duty)
	{
		//Encender salida: PB3 en alto.
		PORTB |= (1 << PB3);
	}
	else
	{
		//Apagar salida: PB3 en bajo.
		PORTB &= ~(1 << PB3);
	}
}

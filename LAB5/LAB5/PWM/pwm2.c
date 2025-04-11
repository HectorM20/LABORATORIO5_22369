#include "pwm2.h"

void initPWM2_FAST(uint8_t invertido, uint16_t prescaler)
{
	//Configurar PB3 como salida para OC2A
	DDRB |= (1 << PB3);
	
	//Limpiar la configuración previa para OC2A en TCCR2A
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));
	
	//Configurar canal OC2A
	if (invertido == PWM2_INVERTIDO)
	{
		TCCR2A |= (1 << COM2A1) | (1 << COM2A0);
	}
	else
	{
		TCCR2A |= (1 << COM2A1);		//Modo no invertido para OC2A
	}
	//Configurar modo Fast PWM: Modo 3 (WGM21=1 y WGM20=1, con WGM22=0)
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2B &= ~(1 << WGM22);
	
	
	TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));	//Limpiar bits del prescaler
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
	    // Si se pasa un valor no soportado, no se configura nada.
	    break;
    }
	//Inicializar OCR2A con el valor inicial
	OCR2A = 8;
}

void updateDutyCycleA2(uint8_t dutyCycle)
{
	OCR2A = dutyCycle;
}

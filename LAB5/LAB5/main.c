///***************************************************************
//Universidad del Valle de Guatemala
//IE2023: Programación de Microcontroladores
//Autor: Héctor Alejandro Martínez Guerra
//Hardware: ATMEGA328P
//LAB5
//***************************************************************


/****************************************/
//Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>				//libreria de interrupciones
#include <util/delay.h>
#include "PWM/pwm0.h"
#include "PWM/timer2PWM.h"
//#include "PWM/pwm2.h"

//Valor inicial para el servo: usamos el valor mínimo (pulso ~1ms)
volatile uint8_t adc_value_A = 0;			//Para servo en OC0A (potenciómetro ADC6)
volatile uint8_t adc_value_B = 0;			//Para servo en OC0B (potenciómetro ADC7)
volatile uint8_t adc_value_C = 0;			//Para led en OC2A (potenciómetro en ADC5)
volatile uint8_t currentADCchannel = 0;		//0: leer ADC6, 1: leer ADC7

// Variables para almacenar los valores mapeados de PWM.
uint8_t servoPWM_A = 0;						//OC0A
uint8_t servoPWM_B = 0;						//OC0B
//uint8_t LED_PWM_C = 0;					//OC2A


/****************************************/
//Function prototypes
void setup();
void initADC();
uint8_t mapADCToServo(uint8_t adc_val);
uint8_t mapADCToLED(uint8_t adc_val);

/****************************************/
//Main Function
int main(void)
{
	setup();
	
    while (1)
    {
	    //Mapear las lecturas ADC a un rango de pulso adecuado para los servos.
	    //Se de 8 a 39 
	    servoPWM_A = mapADCToServo(adc_value_A);
	    servoPWM_B = mapADCToServo(adc_value_B);
		//LED_PWM_C = mapADCToLED(adc_value_C);

	    
	    //Actualizar los duty cycles de cada canal.
	    updateDutyCycleA(servoPWM_A);
	    updateDutyCycleB(servoPWM_B);	
		//updateDutyCycleA2(LED_PWM_C);
		
        //Para el LED se usa un mapeo lineal.
        uint8_t ledDuty = (mapADCToLED(adc_value_C) *100) /255;
        setTimer2Duty(ledDuty);
	    
	    _delay_ms(2);
    }
	
	return 0;
}
/****************************************/
//NON-Interrupt subroutines
void setup()
{
	cli();
	
	//Configurar el reloj: reducir F_CPU a 1MHz (división por 16)
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);				//Divide por 16: 16MHz/16 = 1MHz
	
	//Inicializar PWM en OC0A (PD6) y OC0B
	initPWM0_FAST(PWM0_NO_INVERTIDO, 64);
		
	//Incializar PWM en OC2A
	//initPWM2_FAST(PWM2_NO_INVERTIDO, 32);

    //Inicializar PWM manual para el LED en Timer2
	initTimer2ManualPWM(32, 100);
	
	//Inicializar ADC
	initADC();
	
	sei();
}

/****************************************/
//ADC
void initADC()
{
	ADMUX = 0;
	ADMUX |= (1<<REFS0);			//Voltaje de referencia AVcc
	ADMUX |= (1<<ADLAR);			//Justificación a la izquierda
	ADMUX |= (1<<MUX2) | (1<<MUX1);	//Configurar ADC6 del Arduino nano
	
	ADCSRA = 0;
	//Configurar Prescaler a 8 (1MHz/8 = 125KHz)
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
	//Habilitar ADC y sus interrupciones
	ADCSRA |= (1<<ADEN) | (1<<ADIE);
	
	//Empezar ADC
	ADCSRA |= (1<<ADSC);
}


/****************************************/
//Interrupt routines
ISR(ADC_vect)
{
	if (currentADCchannel == 0)
	{
		//Leer ADC6 y almacenamos en adc_value_A.
		adc_value_A = ADCH;
		//Cambiar a ADC7: Para ADC7 = 0111 (se debe poner MUX0 a 1, MX1 y MUX se dejan)
		ADMUX |= (1 << MUX0);
		currentADCchannel = 1;
	}
	else if (currentADCchannel == 1)
	{
		//Leer ADC7 y almacenamos en adc_value_B.
		adc_value_B = ADCH;
        ADMUX &= ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0));	//Limpiar bits MUX
		ADMUX |= (1 << MUX2) | (1 << MUX0);						//Cambiar a ADC5 0101
		currentADCchannel = 2;	
	}
	else
	{
		//Leer ADC5 y almacenar en adc_value_C
		adc_value_C = ADCH;
		
		//Volver a ADC6 0110
        ADMUX &= ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0));	//limpiar bits MUX
        ADMUX |= (1 << MUX2) | (1 << MUX1);						//0110
        currentADCchannel = 0;
	}
	//Reiniciar la conversión.
	ADCSRA |= (1 << ADSC);
}

//Función que mapea el valor ADC (0-255) al rango de PWM para el servo (16-31)
uint8_t mapADCToServo(uint8_t adc_val)
{
	//Mapear ADC [0,255] a PWM [8,39]
	//Diferencia: 39 - 8 = 31
	uint16_t temp = (uint16_t)adc_val * 31;
	uint8_t mapped = (temp / 255) + 8;
	return mapped;
}

uint8_t mapADCToLED(uint8_t adc_val)
{
	return adc_val;					//rango 0-255.
}


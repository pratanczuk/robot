#include "motors.h"

void motor_1_Forward( unsigned char a_ui8Speed)
{
	OCR1A = a_ui8Speed;
	PORTA |= (1<<PA4); PORTA &= ~(1<<PA5);	
}

void motor_1_Backward( unsigned char a_ui8Speed)
{
	OCR1A = a_ui8Speed;
	PORTA |= (1<<PA5); PORTA &= ~(1<<PA4);
}

void motor_2_Forward( unsigned char a_ui8Speed)
{
	OCR1B = a_ui8Speed;
	PORTA |= (1<<PA6); PORTA &= ~(1<<PA7);
}

void motor_2_Backward( unsigned char a_ui8Speed)
{
	OCR1B = a_ui8Speed;
	PORTA |= (1<<PA7); PORTA &= ~(1<<PA6);
}

void motor_1_Brake( void)
{
	OCR1A=0;
	PORTA &= ~(1<<PA4); PORTA &= ~(1<<PA5);
}

void motor_2_Brake( void)
{
	OCR1B=0;
	PORTA &= ~(1<<PA6); PORTA &= ~(1<<PA7);
}

void motorsInit (void)
{
	// set logic pins as output
	// motor 1
    DDRA |= (1<<PA4);
    DDRA |= (1<<PA5);

	//motor 2
    DDRA |= (1<<PA6);
    DDRA |= (1<<PA7);
	
	/*
	PWM Modes atmega32 for 16 bits timers OC1A and OC1B
	
	Phase and Frequency Correct PWM Mode:
	Phase Correct PWM Mode:
	desired_frequency = (Input_frequency)/(2*Prescaler*TOP_limit)
	TOP_limit = (IF/(2*Prescaler*desired_frequency)) -1
	
	Fast PWM Mode:
	desired_frequency = (Input_frequency)/(Prescaler*(1 + TOP_limit))
	
	Clear Timer on Compare Match (CTC) Mode:
	desired_frequency = (Input_frequency)/(2 * Prescaler *(1 + TOP_limit))
		
    * Prescaler – 1, 2, 4, 8, 16 .. 1024

	*/

// TODO PWM configuration has to be configured verified
	
   	//top=100, prescaler=8, desired_frequency=10 000 hz; Input_frequency=16 000 000 Hz
	ICR1=100;
	TCCR1B|=_BV(CS11);
	
	//phase and frequency correction, icr1 = top
	TCCR1B|=_BV(WGM13);
	
	//set OC1A,OC2B on downcounting, clear on upcounting
	TCCR1A|=_BV(COM1A1)|_BV(COM1B1);
	
	//output pins, PORTD4 (motor 1) and PORTD5 (motor 2)
	DDRD|=(1<<PD4)|(1<<PD5);   //PWM Pins as Out
	
	//default
	motor_1_Brake();
	motor_2_Brake();
}

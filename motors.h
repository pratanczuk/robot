#ifndef __MOTORS__
#define __MOTORS__

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "trace.h"
#include "contract.h"

void motor_1_Forward( unsigned char a_ui8Speed);
void motor_1_Backward( unsigned char a_ui8Speed);
void motor_2_Forward( unsigned char a_ui8Speed);
void motor_2_Backward( unsigned char a_ui8Speed);
void motor_1_Brake( void);
void motor_2_Brake( void);
void motorsInit (void);

#endif /*__MOTORS__*/

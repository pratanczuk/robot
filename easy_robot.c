
/*
History:
20.03.2011:P.Ratanczuk:First release after integration
21.03.2011:P.Ratanczuk:Servo support added, support for engine 5 and 6 added
*/


#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <string.h> 
#include <util/atomic.h>

#define __TRACE__DISABLED__ 1

#include "trace.h"
#include "contract.h"
#include "usart.h"
#include "psx_pad.h"
#include "motors.h"

/*
CPU Ports Mapping for Atmega 32 and Atmega16

Port A
	PA0	(not connected) 
	PA1 (not connected) 
	PA2 (not connected)
	PA3 (not connected) 
	PA4 (connected) Motor 1A
	PA5 (connected) Motor 1B
	PA6 (connected) Motor 2A
	PA7 (connected) Motor 2B

Port B
	PB0 (not connected)
	PB1 (not connected)
	PB2 (not connected) INT2
	PB3 (not connected) PWM
	PB4 (not connected)
	PB5 (connected) MOSI (SPI programmer)
	PB6 (connected) MISO (SPI programmer)
	PB7 (connected) SCK (SPI programmer)

Port C
	PC0 (not connected) I2C SCL 
	PC1 (not connected) I2C SDA
	PC2 (connected) PS2 Pad (ack)
	PC3 (not connected) 
	PC4 (connected) PS2 Pad (command)
	PC5 (connected) PS2 Pad (data)
	PC6 (connected) PS2 Pad (Clock)
	PC7 (connected) PS2 Pad (attention)

Port D
	PD0 (connected) RS232 RX 
	PD1 (connected) RS232 TX
	PD2 (not connected) INT0
	PD3 (not connected) INT1
	PD4 (connected) PWM
	PD5 (connected) PWM
	PD6 (not connected)
	PD7 (not connected) PWM
*/


#define MAX_CMD_SIZE 20
char cmdbuffer[MAX_CMD_SIZE];
char *strchr_pointer; // just a pointer to find chars in the cmd string like X, Y, Z, E, etc
int serial_count = 0;
void process_commands();
int command_complete = 0;



int main ( void)
{
    // init HW uart
    usart_init( 19200);

    // init trace subsystem
    traceInit();

    // Activate interrupts

	sei();

	//init motors
	motorsInit();
	TRACE( "Motors  Init done!");

    TRACE( "System Ready!");
	

    //main loop
    while (1)
    {
	_delay_ms(250);
		
		//flag set by interrupt routine when char received
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 
		{
 			if(command_complete)
			{
				//cli();
				TRACE( "Command read");
				process_commands();
				command_complete = 0;
				serial_count = 0;
				//sei();
			}
		}
    }

}  

int code_seen(char code)
{
  strchr_pointer = strchr(cmdbuffer, code);
  return (strchr_pointer != NULL);  //Return True if a character was found
}
float code_value() { return (strtod(&cmdbuffer[strchr_pointer - cmdbuffer + 1], NULL)); }

void process_commands()
{

  if(code_seen('M') || code_seen('m'))
  {
    switch( (int)code_value() ) 
    {
      case 1: // stop
		motor_1_Brake();
		motor_2_Brake();
		TRACE( "Motor_1,2_BREAK");
       //if (code_seen('S')) numSteps = code_value();
        break;
      case 2: // move forward
		motor_1_Forward( 200);
		motor_2_Forward( 200);
		TRACE( "Motor_1,2_FORWARD");
        break;
	  case 3: // move backward
		motor_1_Backward( 200);
		motor_2_Backward( 200);
	    TRACE( "Motor_1,2_BACKWARD");
        break;
      case 4: // turn left
		motor_2_Forward( 200);
		motor_1_Backward( 200);
	    TRACE( "Motor_2_LEFT");
        break;
      case 5: // turn right
		motor_1_Forward( 200);
		motor_2_Backward( 200);
		TRACE( "Motor_2_Right");
        break;
      default:
	  	TRACE( "Error");
        usart_write_str("Unknown command parameter:");
		usart_write_str(cmdbuffer);
  	    usart_write_str("\n"); 
	    return;
    }

    usart_write_str("ok\n"); 
  }
  else
  {
      usart_write_str("Unknown command:");
      usart_write_str(cmdbuffer);
	  usart_write_str("\n"); 
  }
}


ISR(USART_RXC_vect)
{
	cmdbuffer[serial_count] = UDR;
	if((cmdbuffer[serial_count] == '\n') || (cmdbuffer[serial_count] == '\r') || (serial_count >= (MAX_CMD_SIZE - 1) )) 
	{
		command_complete = 1;
		cmdbuffer[serial_count] = 0;
	}
	else
	{
	serial_count++;
	}	
}

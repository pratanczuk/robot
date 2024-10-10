/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        24.10.2007
 Description:    RS232 Routinen

 Dieses Programm ist freie Software. Sie können es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation veröffentlicht, 
 weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder späteren Version. 

 Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, 
 daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
------------------------------------------------------------------------------*/


#include "usart.h"
#include <avr/io.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <avr/pgmspace.h>
	
//----------------------------------------------------------------------------
//Init serielle Schnittstelle
void usart_init(unsigned long baudrate) 
{ 

	#define PARITY_NONE 0
	#define PARITY_EVEN (1<<UPM1)
	#define PARITY_ODD ((1<<UPM1)|(1<<UPM0))
	#define STOP_BITS_1 0
	#define STOP_BITS_2 (1<<USBS)
	#define DATA_BITS_5 0
	#define DATA_BITS_6 (1<<UCSZ0)
	#define DATA_BITS_7 (1<<UCSZ1)
	#define DATA_BITS_8 ((1<<UCSZ1)|(1<<UCSZ0))
	#define DATA_BITS_9 ((1<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0)) 
	#define SYNCHRONOUS_TRANSMISION (1<<UMSEL)
	#define ASYNCHRONOUS_TRANSMISION (0<<UMSEL)
	
	// take garbages from the port
  	/* Wait latest receive character was pull from UDR */
  	while(UCSRA & (1<<RXC));

  	/* Wait end of transmission */
  	while(!(UCSRA & (1<<UDRE)));

  	/* Disable transmitter and receiver */
  	UCSRB &= ~((1<<RXEN)|(1<<TXEN));

	UBRRH = ((F_CPU / ( baudrate * 16UL) - 1) >>8) ;
	UBRRL = ((F_CPU / ( baudrate * 16UL) - 1)); 
	UCSRA = (0<<U2X); 


	UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|(0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0);

    UCSRB |= (1 << RXEN ) | (1 << TXEN ); // Turn on the transmission and reception circuitry
    UCSRB |= (1 << RXCIE ); // Enable the USART Recieve Complete interrupt (USART_RXC)

}



//----------------------------------------------------------------------------
//Routine für die Serielle Ausgabe eines Zeichens (Schnittstelle0)
void usart_write_char(char c)
{
    //Warten solange bis Zeichen gesendet wurde
    while(!(USR & (1<<UDRE)));
    //Ausgabe des Zeichens
    UDR = c;
	return;
}



//----------------------------------------------------------------------------
//Ausgabe eines Strings
void usart_write_str(char *str)
{
	while (*str)
	{
		usart_write_char(*str++);
	}
}


//------------------------------------------------------------------------------
void sUartPrintf (const char *a_pBuffer,...)
{
	va_list ap;
	va_start (ap, a_pBuffer);	
	
	int format_flag;
	char str_buffer[10];
	char str_null_buffer[10];
	char move = 0;
	char Base = 0;
	int tmp = 0;
	char by;
	char *ptr;
		
	//Ausgabe der Zeichen
    for(;;)
	{
		by = pgm_read_byte(a_pBuffer++);
		if(by==0) break; // end of format string
            
		if (by == '%')
		{
            by = pgm_read_byte(a_pBuffer++);
			if (isdigit(by)>0)
				{
                                 
 				str_null_buffer[0] = by;
				str_null_buffer[1] = '\0';
				move = atoi(str_null_buffer);
                by = pgm_read_byte(a_pBuffer++);
				}

			switch (by)
				{
                case 's':
                    ptr = va_arg(ap,char *);
                    while(*ptr) { usart_write_char(*ptr++); }
                    break;
				case 'b':
					Base = 2;
					goto ConversionLoop;
				case 'c':
					//Int to char
					format_flag = va_arg(ap,int);
					usart_write_char (format_flag++);
					break;
				case 'i':
					Base = 10;
					goto ConversionLoop;
				case 'o':
					Base = 8;
					goto ConversionLoop;
				case 'x':
					Base = 16;
					//****************************
					ConversionLoop:
					//****************************
					itoa(va_arg(ap,int),str_buffer,Base);
					int b=0;
					while (str_buffer[b++] != 0){};
					b--;
					if (b<move)
						{
						move -=b;
						for (tmp = 0;tmp<move;tmp++)
							{
							str_null_buffer[tmp] = '0';
							}
						//tmp ++;
						str_null_buffer[tmp] = '\0';
						strcat(str_null_buffer,str_buffer);
						strcpy(str_buffer,str_null_buffer);
						}
					usart_write_str (str_buffer);
					move =0;
					break;
				}
			
			}	
		else
		{
			usart_write_char( by );	
		}
	}
	va_end(ap);
}




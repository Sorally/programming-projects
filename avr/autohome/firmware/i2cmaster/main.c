/* Name: main.c
 * Author: Robert Altenburg
 * Copyright: (c) 2009 Robert Altenburg 
 * License: Released under GNU GPL v.4 or later
 */

#include <avr/io.h>
#include <util/delay.h>
#include "i2cmaster.h"

#define DEVICE_ID 0x52
#define FLASH_PIN 0x8

void flashLED( unsigned char led_pin);

int main(void)
{
	unsigned char ret;

	i2c_init();			// initialize I2C library

	flashLED(FLASH_PIN);

   ret = i2c_start((DEVICE_ID << 1) +I2C_WRITE);	// set device address and write mode
    if ( ret ) {
			/* failed to issue start condition, possibly no device found */
			i2c_stop();
			for(;;){
				flashLED(FLASH_PIN);
				_delay_ms(50);
			}		
		} else {
			// write something.... 
			i2c_write('H');
			i2c_write('i');
			i2c_stop();			// set stop conditon = release bus
			for(;;){
				flashLED(FLASH_PIN);
				_delay_ms(250);
            }
		}
				
    return 0;               /* never reached */
}

void flashLED(unsigned char led_pin)
{
	DDRB = DDRB | led_pin;		// DDRB preserved except pb3 	 
	PORTB = PORTB | led_pin;	// just pin on
	_delay_ms(250);
	PORTB = PORTB & (!led_pin); // just pin off 
}

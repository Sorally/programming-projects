#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>
#include <string.h>			/* memset(), memcpy() */
#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "i2cmaster.h"


static uchar    reportBuffer[8];    /* buffer for HID reports */
//static uchar    idleRate;           /* in 4 ms units */
static uchar    reportCount = 2;		/* current report */


#define DEVICE_ID 			0x52
#define FLASH_PIN 			0x08

#define VOLUME_UP 			1
#define VOLUME_DOWN			2
#define VOLUME_MUTE			3
#define MEDIA_PLAY			4
#define MEDIA_PAUSE			5
#define MEDIA_NEXT			6
#define MEDIA_PREVIOUS		7

uchar keys[] = {
	0xe9,		// Volume Up
	0xea,		// Volume Down
	0xe2,		// Mute
	0xb0,		// Play
	0xb1,		// Pause
	0xb5,		// Scan Next Track
	0xb6,		// Scan Previous Track
	0xb7,		// Stop
}; 


uchar rx_check_byte = 0;
//static uchar    replyBuffer[8];
unsigned char action = 0;

int tmp = 0;
int temp = 0;
int flag = 0;

PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { // USB report descriptor

    0x05, 0x01,             // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,             // USAGE (Keyboard)
    0xa1, 0x01,             // COLLECTION (Application)
    0x85, 0x01,      		//   REPORT_ID (1)
    0x05, 0x07,             //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,  			//   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,     		//   USAGE_MAXIMUM (Keyboard RightGUI)
    0x15, 0x00,             //   LOGICAL_MINIMUM (0)
    0x25, 0x01,             //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,             //   REPORT_SIZE (1)
    0x95, 0x08,             //   REPORT_COUNT (8)
    0x81, 0x02,             //   INPUT (Data,Var,Abs)
    0x95, 0x06,             //   REPORT_COUNT (6)
    0x75, 0x08,             //   REPORT_SIZE (8)
    0x25, 0x65,             //   LOGICAL_MAXIMUM (101)
    0x19, 0x00,             //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,             //   USAGE_MAXIMUM (101)
    0x81, 0x00,             //   INPUT (Data,Ary,Abs)
    0xc0,					// END_COLLECTION

    0x06, 0x00, 0xff,		// USAGE_PAGE (Generic Desktop)
    0x09, 0x01,				// USAGE (Vendor Usage 1)
    0xa1, 0x01,				// COLLECTION (Application)
	0x85, 0x02,				//   REPORT_ID (2)
    0x15, 0x00,				//   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,		//   LOGICAL_MAXIMUM (255)
    0x75, 0x08,				//   REPORT_SIZE (8)
    0x95, 0x80,				//   REPORT_COUNT (128)
    0x09, 0x00,				//   USAGE (Undefined)
    0xb2, 0x02, 0x01,		//   FEATURE (Data,Var,Abs,Buf)
    0xc0,					// END_COLLECTION

	0x05, 0x0c,				// USAGE_PAGE (Consumer Devices)
	0x09, 0x01,				// USAGE (Consumer Control)
	0xa1, 0x01,				// COLLECTION (Application)
	0x85, 0x03,				//   REPORT_ID (3)
	0x05, 0x0c,				//   USAGE_PAGE (Consumer Devices)
	0x15, 0x01,				//   LOGICAL_MINIMUM (0x01)
	0x25, 0xf0,				//   LOGICAL_MAXIMUM (0xf0)
    0x19, 0x00,             //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0xf0,             //   USAGE_MAXIMUM (101)
	0x75, 0x08,				//   REPORT_SIZE (8)
	0x95, 0x01,				//   REPORT_COUNT (1)
	0x81, 0x40,				//   INPUT (Data,Ary,Abs)
	0xc0,					// END_COLLECTION

};


/* The following variables store the status of the current data transfer */
static uchar    currentAddress;
static uchar    bytesRemaining;

/* ------------------------------------------------------------------------- */

/* usbFunctionRead() is called when the host requests a chunk of data from
 * the device. For more information see the documentation in usbdrv/usbdrv.h.
 */
uchar   usbFunctionRead(uchar *data, uchar len) {

	//i2c_start((DEVICE_ID << 1) + I2C_WRITE);
	//i2c_write(1);
	//i2c_stop();

	//_delay_ms(25);
	//memset(data, 0x31, 6);

//working!
	// get temperature
	i2c_start((DEVICE_ID << 1) + I2C_READ);
	_delay_ms(25);
	data[0] = i2c_read(1);
	data[1] = i2c_read(1);
	data[2] = i2c_read(1);
	data[3] = i2c_read(1);
	data[4] = i2c_read(1);
	data[5] = i2c_read(0);
	i2c_stop();


   return 6;

/*
    if(len > bytesRemaining)
        len = bytesRemaining;
    eeprom_read_block(data, (uchar *)0 + currentAddress, len);
    currentAddress += len;
    bytesRemaining -= len;
    return len;
*/	
}

/* usbFunctionWrite() is called when the host sends a chunk of data to the
 * device. For more information see the documentation in usbdrv/usbdrv.h.
 */
uchar   usbFunctionWrite(uchar *data, uchar len)
{

	if (data[0] == 0) return 1;
	if (data[1] == 0) return 1;

	if (data[1] == 2) { // get current room temperature
		i2c_start((DEVICE_ID << 1) + I2C_WRITE);
		i2c_write(data[1]);
		i2c_stop();

	} else if (data[1] == 3) { // switch aircon ON
		i2c_start((DEVICE_ID << 1) + I2C_WRITE);
		i2c_write(data[1]);
		i2c_stop();

	} else if (data[1] == 4) { // switch aircon OFF
		i2c_start((DEVICE_ID << 1) + I2C_WRITE);
		i2c_write(data[1]);
		i2c_stop();

	} else if (data[1] == 0x05) { // toggle light
		i2c_start((DEVICE_ID << 1) + I2C_WRITE);
		i2c_write(data[1]);
		i2c_stop();

	} else if (data[1] == 0x06) { // switch light
		i2c_start((DEVICE_ID << 1) + I2C_WRITE);
		i2c_write(data[1]);
		i2c_stop();

	}


/*
	PORTB ^= 0b0010;

	i2c_start((DEVICE_ID << 1) + I2C_WRITE);
	i2c_write(0x09);
	i2c_stop();
*/


	return 1;

/*
    if(bytesRemaining == 0)
        return 1;               // end of transfer
    if(len > bytesRemaining)
        len = bytesRemaining;
    eeprom_write_block(data, (uchar *)0 + currentAddress, len);
    currentAddress += len;
    bytesRemaining -= len;
    return bytesRemaining == 0; // return 1 if this was the last chunk
*/
}

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID class request */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* since we have only one report type, we can ignore the report-ID */
            bytesRemaining = 128;
            currentAddress = 0;
            return USB_NO_MSG;  /* use usbFunctionRead() to obtain data */
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
            /* since we have only one report type, we can ignore the report-ID */
            bytesRemaining = 128;
            currentAddress = 0;
            return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
        }
    }else{
        /* ignore vendor type requests, we don't use any */
    }
    return 0;
}


/****************************************************************************
 * buildReport()
 *
 *
 ***************************************************************************/
static void buildReport(void) {

	memset(reportBuffer, 0, 8);

    if(reportCount == 0){
    	reportBuffer[0] = 3;
		reportBuffer[1] = keys[action - 1];
    } else {
	    reportBuffer[0] = 3;
    	reportBuffer[1] = 0;
	}

	action = 0;

	reportCount++;
}





/****************************************************************************
 * main()																	*
 *																			*
 *																			*
 ***************************************************************************/
int main(void) {
	
	uchar i;
	uchar calibrationValue;


    calibrationValue = eeprom_read_byte(0); /* calibration value from last time */
    if(calibrationValue != 0xff){
        OSCCAL = calibrationValue;
    }

	i2c_init();
	
    usbInit();
    usbDeviceDisconnect(); 

    i = 0;
    while(--i) {
        _delay_ms(1);
    }

    usbDeviceConnect();

	DDRB &= ~_BV(DDB1);			// input from slave to indicate a msg is waiting

/////////
//	DDRB = 0b0010; // set pb1 as output
/////////
	
	sei();

    for(;;) {

        usbPoll();

		if (PINB & _BV(PB1)) {
			while (PINB & _BV(PB1)) {
				// wait until off
			}

			i2c_start((DEVICE_ID << 1) + I2C_READ);
			action = i2c_read(0);
			i2c_stop();

			if (action <= sizeof(keys)/sizeof(uchar)) {
				reportCount = 0;
			}
		}

		if(usbInterruptIsReady() && (reportCount < 2)){ /* we can send another key */
        	buildReport();
           	usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
        }

    }

    return 0;
}



/*
working

    0x05, 0x01,             // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,             // USAGE (Keyboard)
    0xa1, 0x01,             // COLLECTION (Application)
    0x85, 0x01,      		//   REPORT_ID (1)
    0x05, 0x07,             //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,  			//   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,     		//   USAGE_MAXIMUM (Keyboard RightGUI)
    0x15, 0x00,             //   LOGICAL_MINIMUM (0)
    0x25, 0x01,             //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,             //   REPORT_SIZE (1)
    0x95, 0x08,             //   REPORT_COUNT (8)
    0x81, 0x02,             //   INPUT (Data,Var,Abs)
    0x95, 0x06,             //   REPORT_COUNT (6)
    0x75, 0x08,             //   REPORT_SIZE (8)
    0x25, 0x65,             //   LOGICAL_MAXIMUM (101)
    0x19, 0x00,             //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,             //   USAGE_MAXIMUM (101)
    0x81, 0x00,             //   INPUT (Data,Ary,Abs)
    0xc0,					// END_COLLECTION


	0x05, 0x0c,				// USAGE_PAGE (Consumer Devices)
	0x09, 0x01,				// USAGE (Consumer Control)
	0xa1, 0x01,				// COLLECTION (Application)
	0x85, 0x02,				//   REPORT_ID (2)
	0x05, 0x0c,				//   USAGE_PAGE (Consumer Devices)
	0x15, 0x01,				//   LOGICAL_MINIMUM (0x01)
	0x25, 0xf0,				//   LOGICAL_MAXIMUM (0xf0)
    0x19, 0x00,             //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0xf0,             //   USAGE_MAXIMUM (101)
	0x75, 0x08,				//   REPORT_SIZE (8)
	0x95, 0x01,				//   REPORT_COUNT (1)
	0x81, 0x40,				//   INPUT (Data,Ary,Abs)
	0xc0,					// END_COLLECTION

*/

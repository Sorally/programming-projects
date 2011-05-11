/* Name: hidtool.c
 * Project: hid-data example
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: hidtool.c 723 2009-03-16 19:04:32Z cs $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hiddata.h"
#include "../firmware/usbconfig.h"  /* for device VID, PID, vendor name and product name */

extern void timestamp(void);


static char *usbErrorMessage(int errCode) {
	static char buffer[80];

	switch(errCode){
		case USBOPEN_ERR_ACCESS:      return "Access to device denied";
		case USBOPEN_ERR_NOTFOUND:    return "The specified device was not found";
		case USBOPEN_ERR_IO:          return "Communication error with device";
		default:
			sprintf(buffer, "Unknown USB error %d", errCode);
			return buffer;
	}
	return NULL;    /* not reached */
}

static usbDevice_t  *openDevice(int useReportIds) {
	usbDevice_t *dev = NULL;
	unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
	char vendorName[] = {USB_CFG_VENDOR_NAME, 0}, productName[] = {USB_CFG_DEVICE_NAME, 0};
	int vid = rawVid[0] + 256 * rawVid[1];
	int pid = rawPid[0] + 256 * rawPid[1];
	int err;

	//printf("%susing useReportIds\n", useReportIds?"":"not ");

	if((err = usbhidOpenDevice(&dev, vid, vendorName, pid, productName, useReportIds)) != 0){
		timestamp();
		printf("error finding %s: %s\n", productName, usbErrorMessage(err));
		return NULL;
	}
	return dev;
}


static int  hexread(char *buffer, char *string, int buflen) {
	char    *s;
	int     pos = 0;

	while((s = strtok(string, ", ")) != NULL && pos < buflen){
		string = NULL;
		buffer[pos++] = (char)strtol(s, NULL, 0);
	}
	return pos;
}


static void usage(char *myName) {
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "  %s read reportID\n", myName);
	fprintf(stderr, "  %s write <listofbytes>\n", myName);
}

int interface(int argc, char **argv, char *temperature) {
	usbDevice_t *dev;
	char buffer[129];    /* room for dummy report ID */
	int err;
	int reportID = 0;
	int useReportIds = 1;

	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}


	reportID = argv[2][0] - 0x30;
	
	if (reportID == 0) {
		useReportIds = 0;
	}
    
	if((dev = openDevice(useReportIds)) == NULL) {
			timestamp();
			printf("aborting\n");
			return 0;
		}

	if(strcasecmp(argv[1], "read") == 0){
		int len = sizeof(buffer);
		memset(buffer, 0, sizeof(buffer));
		if((err = usbhidGetReport(dev, reportID, buffer, &len)) != 0){
			fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
		} else {
			buffer[5] = 0;
			memcpy(temperature, buffer, 6);
			usbhidCloseDevice(dev);
			return 0;
		}
	}else if(strcasecmp(argv[1], "write") == 0){
		int i, pos;
		memset(buffer, 0, sizeof(buffer));
		buffer[0] = 2;
		for(pos = 1, i = 2; i < argc && pos < sizeof(buffer); i++){
			pos += hexread(buffer + pos, argv[i], sizeof(buffer) - pos);
		}
		if((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) {   /* add a dummy report ID */
			timestamp();
			printf("error writing data: %s\n", usbErrorMessage(err));
		}
	} else {
		usage(argv[0]);
		return 0;
	}
	usbhidCloseDevice(dev);
	return 0;
}

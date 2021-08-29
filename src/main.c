/*
 *  ventusx: controls ttesports ventus x mice
 *  Copyright (C) 2021 Pigpog
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define HID_GET_REPORT                0x01
#define HID_SET_REPORT                0x09
#define HID_REPORT_TYPE_FEATURE       0x03

const static uint16_t VENDOR_ID = 0x264a;
const static uint16_t PRODUCT_ID = 0x1007;
const static uint16_t PACKET_CTRL_LEN = 8;

static struct libusb_device_handle *devh = NULL;

static void cleanup() {
	libusb_release_interface(devh, 0);
	libusb_attach_kernel_driver(devh, 0);
	libusb_close(devh);
	libusb_exit(NULL);
}

static int setup() {
	int state = 1;

	state = libusb_init(NULL);
	if (state < 0) {
		fprintf(stderr, "failed to initialize libusb\n");
		return 1;
	}

	// open the mouse device using it's vendor id and product id
	devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
	if (devh == NULL) {
		fprintf(stderr, "did not detect a ventus x mouse. are you root?\n");
		return 1;
	}

	libusb_set_auto_detach_kernel_driver(devh, 0);

	// if a kernel driver is attached, detach it
	if (libusb_kernel_driver_active(devh, 0) != 0) {
		fprintf(stderr, "other driver is in use; detaching.\n");
		state = libusb_detach_kernel_driver(devh, 0);
		if (state != 0) {
			fprintf(stderr, "failed to detach kernel driver with error: %d\n", state);
			return 1;
		}
	}
	
	state = libusb_claim_interface(devh, 0);
	if (state < 0) {
		fprintf(stderr, "libusb_claim_interface failed with code %d\n", state);
		return 1;
	}

	printf("claimed interface; setup complete.\n");

	return 0;
}

void usage(char* basename) {
	printf("usage: %s {rear | scroll} {on | off}\n", basename);
}

int main(int argc, char** argv) {
	/* observed values:
	 * rear light off   = data c40f001300000000 length 8 wIndex 0 wValue 0x0300 bRequest 9 bRequest 0x09 bmRequestType 0x21
	 * rear light on    = data c40f001302000000 length 8 wIndex 0 wValue 0x0300 bRequest 9 bRequest 0x09 bmRequestType 0x21
	 * scroll light off = data c40f001500000000 length 8 wIndex 0 wValue 0x0300 bRequest 9 bRequest 0x09 bmRequestType 0x21
	 * scroll light on  = data c40f001501000000 length 8 wIndex 0 wValue 0x0300 bRequest 9 bRequest 0x09 bmRequestType 0x21
	 */
	
	unsigned char *data;
	unsigned char rear_off[] =   { 0xc4, 0x0f, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char rear_on[] =    { 0xc4, 0x0f, 0x00, 0x13, 0x02, 0x00, 0x00, 0x00, 0x00 };
	unsigned char scroll_off[] = { 0xc4, 0x0f, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char scroll_on[] =  { 0xc4, 0x0f, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00 };

	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}

	// cli arg handling
	if (strcmp(argv[1], "rear") == 0) {
		if (strcmp(argv[2] , "off") == 0) {
			data = rear_off;
		} else if (strcmp(argv[2], "on") == 0) {
			data = rear_on;
		} else {
			usage(argv[0]);
			exit(1);
		}
	} else if (strcmp(argv[1], "scroll") == 0) {
		if (strcmp(argv[2], "off") == 0) {
			data = scroll_off;
		} else if (strcmp(argv[2], "on") == 0) {
			data = scroll_on;
		} else {
			usage(argv[0]);
			exit(1);
		}
	} else {
		usage(argv[0]);
		exit(1);
	}

	if (setup() != 0) {
		exit(1);
		cleanup();
	}

	// send the command to the mouse
	int state = libusb_control_transfer(devh, 0x21, HID_SET_REPORT, 0x0300, 0, data, PACKET_CTRL_LEN, 5000);
	if (state < 0) {
		fprintf(stderr, "control out error %d\n", state);
		cleanup();
		exit(1);
	}

	cleanup();
	return 0;
}

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

static void usage(char* basename) {
	printf("usage: %s {palm | scroll} {on | off | battle | pulse}\n", basename);
}

static void send_command(unsigned char *data) {
	// send the command to the mouse
	int state = libusb_control_transfer(devh, 0x21, HID_SET_REPORT, 0x0300, 0, data, PACKET_CTRL_LEN, 5000);
	if (state < 0) {
		fprintf(stderr, "control out error %d\n", state);
		cleanup();
		exit(1);
	}
}

int main(int argc, char** argv) {
	unsigned char led_palm_off[] =      { 0xc4, 0x0f, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00 };
	unsigned char led_palm_on[] =       { 0xc4, 0x0f, 0x00, 0x13, 0x01, 0x00, 0x00, 0x00 };
	unsigned char led_palm_pulse[] =    { 0xc4, 0x0f, 0x00, 0x13, 0x02, 0x00, 0x00, 0x00 };
	unsigned char led_palm_battle[] =   { 0xc4, 0x0f, 0x00, 0x13, 0x03, 0x00, 0x00, 0x00 };

	unsigned char led_scroll_off[] =    { 0xc4, 0x0f, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00 };
	unsigned char led_scroll_on[] =     { 0xc4, 0x0f, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00 };
	unsigned char led_scroll_pulse[] =  { 0xc4, 0x0f, 0x00, 0x15, 0x02, 0x00, 0x00, 0x00 };
	unsigned char led_scroll_battle[] = { 0xc4, 0x0f, 0x00, 0x15, 0x03, 0x00, 0x00, 0x00 };

	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}

	if (setup() != 0) {
		exit(1);
		cleanup();
	}

	// cli arg handling
	if (strcmp(argv[1], "palm") == 0) {
		if (strcmp(argv[2] , "off") == 0) {
			send_command(led_palm_off);
		} else if (strcmp(argv[2], "on") == 0) {
			send_command(led_palm_on);
		} else if (strcmp(argv[2], "battle") == 0) {
			send_command(led_palm_battle);
		} else if (strcmp(argv[2], "pulse") == 0) {
			send_command(led_palm_pulse);
		} else {
			usage(argv[0]);
			exit(1);
		}
	} else if (strcmp(argv[1], "scroll") == 0) {
		if (strcmp(argv[2], "off") == 0) {
			send_command(led_scroll_off);
		} else if (strcmp(argv[2], "on") == 0) {
			send_command(led_scroll_on);
		} else if (strcmp(argv[2], "battle") == 0) {
			send_command(led_scroll_battle);
		} else if (strcmp(argv[2], "pulse") == 0) {
			send_command(led_scroll_pulse);
		} else {
			usage(argv[0]);
			exit(1);
		}
	} else {
		usage(argv[0]);
		exit(1);
	}

	cleanup();
	return 0;
}

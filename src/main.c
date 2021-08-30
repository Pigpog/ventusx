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
	if (devh != NULL) libusb_release_interface(devh, 0);
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
	printf("usage: %s [{palm|scroll} {on|off|battle|pulse|{brightness VALUE}}]\n", basename);
	cleanup();
	exit(1);
}

static void send_command(const unsigned char *address, const unsigned char *value) {
	unsigned char data[PACKET_CTRL_LEN];
	// combine address and value
	for (int i = 0; i < 4; i++){
		data[i] = address[i];
		data[i + 4] = value[i];
	}

	// send the command to the mouse
	int state = libusb_control_transfer(devh, 0x21, HID_SET_REPORT, 0x0300, 0, data, PACKET_CTRL_LEN, 5000);
	if (state < 0) {
		fprintf(stderr, "control out error %d\n", state);
		cleanup();
		exit(1);
	}
}

int main(int argc, char** argv) {
	// addresses
	const unsigned char led_palm[] =              { 0xc4, 0x0f, 0x00, 0x13 };
	const unsigned char led_palm_brightness[] =   { 0xc4, 0x0f, 0x00, 0x14 };
	const unsigned char led_scroll[] =            { 0xc4, 0x0f, 0x00, 0x15 };
	const unsigned char led_scroll_brightness[] = { 0xc4, 0x0f, 0x00, 0x16 };

	const unsigned char x_dpi[] = { 0xc4, 0x0f, 0x00, 0x04 };
	const unsigned char y_dpi[] = { 0xc4, 0x0f, 0x00, 0x05 };

	// not entirely sure what this is yet, but it makes it work
	const unsigned char save[] = { 0xc4, 0x0f, 0x01, 0x00 };


	// values
	const unsigned char led_off[] =    { 0x00, 0x00, 0x00, 0x00 };
	const unsigned char led_on[] =     { 0x01, 0x00, 0x00, 0x00 };
	const unsigned char led_pulse[] =  { 0x02, 0x00, 0x00, 0x00 };
	const unsigned char led_battle[] = { 0x03, 0x00, 0x00, 0x00 };
	const unsigned char nothing[] =    { 0x00, 0x00, 0x00, 0x00 };

	// stores user input values
	unsigned char hold[] = { 0x00, 0x00, 0x00, 0x00 };

	if (argc < 3) usage(argv[0]);

	if (setup() != 0) {
		cleanup();
		exit(1);
	}

	// cli arg handling
	if (strcmp(argv[1], "palm") == 0) {
		if (strcmp(argv[2] , "off") == 0) {
			send_command(led_palm, led_off);
		} else if (strcmp(argv[2], "on") == 0) {
			send_command(led_palm, led_on);
		} else if (strcmp(argv[2], "battle") == 0) {
			send_command(led_palm, led_battle);
		} else if (strcmp(argv[2], "pulse") == 0) {
			send_command(led_palm, led_pulse);
		} else if (strcmp(argv[2], "brightness") == 0) {
			if (argc < 4) usage(argv[0]);
			sscanf(argv[3], "%hhu", &hold[0]);
			send_command(led_palm_brightness, hold);
		} else {
			usage(argv[0]);
		}
	} else if (strcmp(argv[1], "scroll") == 0) {
		if (strcmp(argv[2], "off") == 0) {
			send_command(led_scroll, led_off);
		} else if (strcmp(argv[2], "on") == 0) {
			send_command(led_scroll, led_on);
		} else if (strcmp(argv[2], "battle") == 0) {
			send_command(led_scroll, led_battle);
		} else if (strcmp(argv[2], "pulse") == 0) {
			send_command(led_scroll, led_pulse);
		} else if (strcmp(argv[2], "brightness") == 0) {
			if (argc < 4) usage(argv[0]);
			sscanf(argv[3], "%hhu", &hold[0]);
			send_command(led_scroll_brightness, hold);
		} else {
			usage(argv[0]);
		}
	} else if (strcmp(argv[1], "dpi") == 0) {
		if (argc < 3) usage(argv[0]);
		sscanf(argv[2], "%hhu", &hold[0]);
		send_command(x_dpi, hold);
		send_command(y_dpi, hold);
		send_command(save, nothing);
	} else {
		usage(argv[0]);
	}

	cleanup();
	return 0;
}

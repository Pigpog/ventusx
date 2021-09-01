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
#include "data.c"

#define HID_GET_REPORT                0x01
#define HID_SET_REPORT                0x09
#define HID_REPORT_TYPE_FEATURE       0x03

const static uint16_t VENDOR_ID = 0x264a;
const static uint16_t PRODUCT_ID = 0x1007;
const static uint16_t PACKET_CTRL_LEN = 8;

static char* basename;

// not entirely sure what this is yet, but it makes it work
unsigned char save[] = { 0xc4, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

static struct libusb_device_handle *devh = NULL;

static void cleanup() {
	if (devh != NULL) {
		libusb_release_interface(devh, 0);
		libusb_attach_kernel_driver(devh, 0);
	}
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

static void usage() {
	printf("usage:\n");
	printf("    control an led: %s {palm|scroll} {on|off|battle|pulse|{brightness VALUE}}\n", basename);
	printf("    set dpi: %s dpi VALUE\n", basename);
	printf("    set polling rate: %s polling VALUE\n", basename);
	printf("    rebind a buttion: %s bind BTTN {kbd|mouse} VALUE\n", basename);
	cleanup();
	exit(1);
}

static void send_command(const unsigned char address, const unsigned char value) {
	int state;
	unsigned char data[] = { 0xc4, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	// combine address and value
	data[3] = address;
	data[4] = value;

	// send the command to the mouse
	state = libusb_control_transfer(devh, 0x21, HID_SET_REPORT, 0x0300, 0, data, PACKET_CTRL_LEN, 5000);
	if (state < 0) {
		fprintf(stderr, "control out error %d\n", state);
		cleanup();
		exit(1);
	}

	state = libusb_control_transfer(devh, 0x21, HID_SET_REPORT, 0x0300, 0, save, PACKET_CTRL_LEN, 5000);
	if (state < 0) {
		fprintf(stderr, "control out error %d\n", state);
		cleanup();
		exit(1);
	}
}

int main(int argc, char** argv) {
	basename = argv[0];

	// the dest addr of the command
	unsigned char addr;
	// stores user input values
	unsigned char value = 0x00;

	if (argc < 3) usage();

	if (setup() != 0) {
		cleanup();
		exit(1);
	}

	// cli arg handling
	if (strcmp(argv[1], "palm") == 0 || strcmp(argv[1], "scroll") == 0) {
		if (strcmp(argv[1], "palm") == 0) addr = led_palm;
		else if (strcmp(argv[1], "scroll") == 0) addr = led_scroll;

		if (strcmp(argv[2], "off") == 0)		value = led_off;
		else if (strcmp(argv[2], "on") == 0)		value = led_on;
		else if (strcmp(argv[2], "battle") == 0)	value = led_battle;
		else if (strcmp(argv[2], "pulse") == 0)		value = led_pulse;
		else if (strcmp(argv[2], "brightness") == 0) {
			if (argc < 4) usage();
			// brightness addr = led addr + 1
			addr++;
			sscanf(argv[3], "%hhu", &value);
		} else usage();

		send_command(addr, value);
	} else if (strcmp(argv[1], "dpi") == 0) {
		sscanf(argv[2], "%hhu", &value);
		send_command(x_dpi, value);
		send_command(y_dpi, value);
	} else if (strcmp(argv[1], "polling") == 0) {
		sscanf(argv[2], "%hhu", &value);
		send_command(polling_rate, value);
	} else if (strcmp(argv[1], "bind") == 0) {
		if (argc < 5) usage();

		if (strcmp(argv[2], "left") == 0)		addr = bind_left_btn;
		else if (strcmp(argv[2], "right") == 0)		addr = bind_right_btn;
		else if (strcmp(argv[2], "scroll") == 0)	addr = bind_scroll_btn;
		else if (strcmp(argv[2], "dpi") == 0)		addr = bind_macro_btn;
		else if (strcmp(argv[2], "scroll_up") == 0)	addr = bind_scroll_up;
		else if (strcmp(argv[2], "scroll_down") == 0)	addr = bind_scroll_dwn;
		else if (strcmp(argv[2], "forward") == 0)	addr = bind_fwd_btn;
		else if (strcmp(argv[2], "back") == 0)		addr = bind_back_btn;
		else usage();

		if (strcmp(argv[3], "mouse") == 0) {
			// set the button to perform mouse functions
			send_command(addr, 0x01);
			// clear kbd bind
			send_command(addr + 2, 0x00);

			value = resolve_bind(argv[4], 0);
			if (value == 0) {
				fprintf(stderr, "cannot bind '%s'\n", argv[4]);
				exit(1);
			}

			addr++;
		} else if (strcmp(argv[3], "kbd") == 0) {
			// set the button to perform kbd functions
			send_command(addr, 0x02);
			// clear mouse bind
			send_command(addr + 1, 0x00);

			value = resolve_bind(argv[4], 1);
			if (value == 0) { 
				fprintf(stderr, "cannot bind '%s'\n", argv[4]);
				exit(1);
			}

			addr += 2;
		} else usage();

		send_command(addr, value);
	} else {
		usage();
	}

	cleanup();
	return 0;
}

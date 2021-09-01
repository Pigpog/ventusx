#define HID_GET_REPORT                0x01
#define HID_SET_REPORT                0x09
#define HID_REPORT_TYPE_FEATURE       0x03

const static uint16_t VENDOR_ID = 0x264a;
const static uint16_t PRODUCT_ID = 0x1007;
const static uint16_t PACKET_CTRL_LEN = 8;

// not entirely sure what this is yet, but it makes it work
unsigned char save[] = { 0xc4, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

// -- addresses --
// led control addresses
const unsigned char led_palm =   0x13;
const unsigned char led_scroll = 0x15;

// performance control addresses
const unsigned char x_dpi = 0x04;
const unsigned char y_dpi = 0x05;

// values: 0x01 = 1000Hz, 0x02 = 500Hz, 0x04 = 250Hz,  0x08 = 125Hz
const unsigned char polling_rate = 0x00; 

// bind control addresses
// these addresses set whether the bind is
// a mouse or kbd function.
// sending 0x01 sets it to mouse
// sending 0x02 sets it to kbd
// addr + 1 = the mouse mode bind
// addr + 2 = the kbd mode bind
const unsigned char bind_macro_btn =  0x30;
const unsigned char bind_left_btn =   0x17;
const unsigned char bind_right_btn =  0x1c;
const unsigned char bind_scroll_btn = 0x21;
const unsigned char bind_scroll_up =  0x35;
const unsigned char bind_scroll_dwn = 0x3a;
const unsigned char bind_back_btn =   0x26;
const unsigned char bind_fwd_btn =    0x2b;

// -- values --
const unsigned char led_off =    0x00;
const unsigned char led_on =     0x01;
const unsigned char led_pulse =  0x02;
const unsigned char led_battle = 0x03;

const char *polling_rates[] = { "", "1000", "", "500", "", "250", "", "125"};

// keycodes for keyboard-mode bindings
// the index of each item is it's keycode
// blank entries are unknown or unused
const char *kbd_bind_map[] = 
{ "" , "" , "" , "" , "a", "b", "c", "d",
  "e", "f", "g", "h", "i", "j", "k", "l",
  "m", "n", "o", "p", "q", "r", "s", "t",
  "u", "v", "w", "x", "y", "z", "1", "2",
  "3", "4", "5", "6", "7", "8", "9", "0",
  "Return" , "Escape", "BackSpace", "Tab", " ", "-", "=", "(",
  ")", "\\", "\\", ";", "'", "`", ",", ".",
  "/", "" , "F1", "F2", "F3", "F4", "F5", "F6",
  "F7", "F8", "F9", "F10", "F11", "F12", "Print", "Scroll_Lock",
  "Pause", "Insert", "Home", "Prior", "Delete", "End",
  "Next", "Right", "Left", "Down", "Up", "Num_Lock", "KP_Divide", "KP_Multiply", "KP_Subtract",
  "KP_Add", "KP_Enter", "KP_1", "KP_2", "KP_3", "KP_4", "KP_5",
  "KP_6", "KP_7", "KP_8", "KP_9", "KP_0", "KP_Decimal", "less", "Menu",
  "Shutdown", "KP_Equals", "XF86Tools", "XF86Launch5", "XF86Launch6", "XF86Launch7", "XF86Launch8", "XF86Launch9",
  "", "XF86AudioMicMut", "XF86TouchpadTog", "XF86TouchpadOn", "XF86TouchpadOff", "", "XF86Open", "Help",
  "SunProps", "SunFront", "Cancel", "Redo", "Undo", "XF86Cut", "XF86Copy", "XF86Paste",
  "Find", "XF86AudioMute", "", "", "", "", "", "KP_Decimal",
  "", "", "Hiragana_Kataka", "", "Henkan_Mode", "Muhenkan", "", ""
  "", "", "Hangul", "Hangul_Hanja", "Katakana", "Hiragana", "", "", 
  "", "", "", "", "", "", "Delete", "", 
  "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "",
  "", "parenleft", "parenright", "", "", "", "", "",
  "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "Delete", "", "", "", "", 
  "", "", "", "Control_L", "Shift_L", "Alt_L", "Super_L", "Control_R", 
  "Shift_R", "Alt_R", "Super_R", "", "XF86AudioStop", "", "", "XF86Eject",
  "", "", "XF86AudioMute", "XF86WWW", "XF86Back", "XF86Forward", "Cancel", "Find", 
  "XF86ScrollUp", "XF86ScrollDown", "", ""
};

const char *mouse_bind_map[] = { "", "left", "right", "", "scroll" };

unsigned char resolve_bind(char *input, int mode) {
	if (mode == 1){
		for(int i = 0; i < sizeof(*kbd_bind_map); i++) {
			if (strcmp(kbd_bind_map[i], input) == 0) return i;
		}
	} else {
		for(int i = 0; i < sizeof(*mouse_bind_map); i++) {
			if (strcmp(mouse_bind_map[i], input) == 0) return i;
		}
	}
	return 0;
}

unsigned char resolve_polling_rate(char *input) {
	for(int i = 0; i < sizeof(*polling_rates); i++) {
		if (strcmp(polling_rates[i], input) == 0) return i;
	}
	return 255;
}

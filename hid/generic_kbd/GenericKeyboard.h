#ifndef GENERICKEYBOARD_H
#define GENERICKEYBOARD_H

#include <HIDDevice.h>

#define DATA_PORT 0x60
#define STATUS_REG 0x64
#define CMD_REG 0x64

// Taken from here: http://wiki.osdev.org/PS/2_Keyboard
#define CMD_SET_LEDS 0xED
#define CMD_SET_REPEAT 0xF3
#define CMD_SET_DEFAULT_PARAM 0xF6
#define CMD_RESET_AND_SELFTEST 0xFF
#define CMD_ENABLE_SCANNING 0xF4
#define CMD_DISABLE_SCANNING 0xF5
#define CMD_IDENTIFY 0xF2

#define DEV_SELFTEST_PASSED 0xAA
#define DEV_SELFTEST_FAILED1 0xFC
#define DEV_SELFTEST_FAILED2 0xFD
#define DEV_RESEND 0xFE
#define DEV_ACK 0xFA

class GenericKeyboard : public IO::HIDDevice
{
	const char* m_name;
	void sendCommand(unsigned char cmd);
	void clearBuffer();
	
	bool m_leds[3];
	
public:
	GenericKeyboard() : IO::HIDDevice(), m_name("Generic Keyboard") {}
	bool setLed(int idx, bool value);
	bool initialize();
	bool handle(message_t& msg);
	const char* getName() { return m_name; }
	const char* getGenericName() { return "keyboard"; }
};

#endif // GENERICKEYBOARD_H

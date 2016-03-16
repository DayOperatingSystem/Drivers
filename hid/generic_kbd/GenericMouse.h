#ifndef GENERICMOUSE_H
#define GENERICMOUSE_H

#include <HIDDevice.h>

#define DATA_PORT 0x60
#define STATUS_REG 0x64
#define CMD_REG 0x64

#define CMD_RESET 0xFF
#define CMD_SET_DEFAULTS 0xF6
#define CMD_DISABLE_REPORTING 0xF5
#define CMD_ENABLE_REPORTING 0xF4
#define CMD_SET_SAMPLE_RATE 0xF3
#define CMD_GET_DEVICE_ID 0xF2
#define CMD_SET_REMOTE_MODE 0xF0
#define CMD_SET_WRAP_MODE 0xEE
#define CMD_RESET_WRAP_MODE 0xEC
#define CMD_READ_DATA 0xEB
#define CMD_SET_STREAM_MODE 0xEA
#define CMD_REQUEST_STATUS 0xE9

#define DEV_RESEND 0xFE
#define DEV_ACK 0xFA

class GenericMouse : public IO::HIDDevice
{
	const char* m_name;
	void sendCommand(unsigned char cmd);
	unsigned char readByte();
	void clearBuffer();
	
	void waitWrite();
	void waitRead();
	
	bool m_buttons[3];
	char m_data[3];
	unsigned int m_state;
	
public:
	GenericMouse() : IO::HIDDevice(), m_name("Generic Mouse") {}
	bool initialize();
	bool handle(message_t& msg);
	const char* getName() { return m_name; }
	const char* getGenericName() { return "mouse"; }
};

#endif // GENERICMOUSE_H

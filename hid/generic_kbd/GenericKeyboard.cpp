#include "GenericKeyboard.h"

void outb(unsigned short port, unsigned char value)
{
	asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

unsigned char inb(unsigned short port)
{
	unsigned char ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

void GenericKeyboard::sendCommand(unsigned char cmd)
{
	while((inb(STATUS_REG) & 0x2));
	outb(DATA_PORT, cmd);
}

void GenericKeyboard::clearBuffer()
{
	while(inb(STATUS_REG) & 0x1) inb(DATA_PORT);
}

#define ISSUE_COMMAND(cmd) { sendCommand(cmd); if(inb(0x60) != DEV_ACK) { debug_printf("[ KBD ] Device did not reply to " #cmd " command!\n"); return false; }}

bool GenericKeyboard::initialize()
{
	clearBuffer();
	
	// Disable scanning so it does not interfere with our
	// detection code
	ISSUE_COMMAND(CMD_DISABLE_SCANNING);
	ISSUE_COMMAND(CMD_IDENTIFY);
	
	// A default AT keyboard does not send
	// any further info
	if((inb(STATUS_REG) & 0x1) == 0)
	{
		// Re-enable keyboard scanning
		ISSUE_COMMAND(CMD_ENABLE_SCANNING);
		m_name = "AT Keyboard";
		return true;
	}
	
	unsigned char result = inb(DATA_PORT);
	debug_printf("[ KBD ] Found PS/2 device with ID: 0x%x\n", result);
	
	// Check if it is a MF2 kbd
	if(result == 0xAB)
	{
		result = inb(DATA_PORT);
		if(result == 0x41 || result == 0xC1)
		{
			// Re-enable keyboard scanning
			ISSUE_COMMAND(CMD_ENABLE_SCANNING);
			
			m_name = "MF2 Keyboard with Translation";
			return true;
		}
		else if(result == 0x83)
		{
			// Re-enable keyboard scanning
			ISSUE_COMMAND(CMD_ENABLE_SCANNING);
		
			m_name = "MF2 Keyboard";
			return true;
		}
	}
	
	return false;
}


bool GenericKeyboard::setLed(int idx, bool value)
{
	m_leds[idx] = value;
	unsigned char cmd = 0;
	
	// Scroll Lock
	cmd |= m_leds[0];
	
	// Number Lock
	cmd |= m_leds[1] << 0x1;
		
	// Caps Lock
	cmd |= m_leds[2] << 0x2;
	
	ISSUE_COMMAND(CMD_SET_LEDS);
	ISSUE_COMMAND(cmd);
	
	return true;
}


bool GenericKeyboard::handle(message_t& msg)
{
	if(msg.signal != 0x21)
		return HIDDevice::handle(msg);
	
	IO::HIDEvent event;
	unsigned char scancode = inb(DATA_PORT);
	
	event.type = (scancode & 0x80) ? IO::HID_KEY_UP : IO::HID_KEY_DOWN;
	event.data = (scancode & ~0x80);
	
	notifyObservers(event);
	return true;
}

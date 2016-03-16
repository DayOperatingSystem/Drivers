#include "GenericMouse.h"
#include <cstring>

void outb(unsigned short port, unsigned char value);
unsigned char inb(unsigned short port);

void GenericMouse::sendCommand(unsigned char cmd)
{
	waitWrite();
	outb(0x64, 0xD4);
	waitWrite();
	
	outb(DATA_PORT, cmd);
}

unsigned char GenericMouse::readByte()
{
	waitRead();
	return inb(DATA_PORT);
}

void GenericMouse::clearBuffer()
{
	while(inb(STATUS_REG) & 0x1) inb(DATA_PORT);
}


void GenericMouse::waitWrite()
{
	static int delay = 100;
	while((inb(0x64) & 0x2) && delay--) sleep(1);
}
	
void GenericMouse::waitRead()
{
	static int delay = 100;
	while((inb(0x64) & 0x1) == 0 && delay--) sleep(1);
}

#define ISSUE_COMMAND(cmd) { sendCommand(cmd); if(inb(0x60) != DEV_ACK) { debug_printf("[ MOUSE ] Device did not reply to " #cmd " command!\n"); return false; }}

bool GenericMouse::initialize()
{
	clearBuffer();
	
	waitWrite();
	outb(0x64, 0xA8);
	
	waitWrite();
	outb(0x64, 0x20);
	waitRead();
	
	unsigned char status = inb(DATA_PORT) | 0x2;
	waitWrite();
	
	outb(0x64, 0x60);
	waitWrite();
	
	outb(DATA_PORT, status);
	
	ISSUE_COMMAND(CMD_SET_DEFAULTS);
	readByte();
	
	ISSUE_COMMAND(CMD_ENABLE_REPORTING);
	
	ISSUE_COMMAND(CMD_GET_DEVICE_ID);
	unsigned char id = inb(DATA_PORT);
	
	debug_printf("[ MOUSE ] Found mouse with ID 0x%x\n", (int) id);
	m_state = 0;
	
	memset(m_buttons, 0, sizeof(m_buttons));
	return true;
}

bool GenericMouse::handle(message_t& msg)
{
	if(msg.signal != 0x20 + 12)
		return HIDDevice::handle(msg);
	
	// Check that we should handle the interrupt
	if((inb(0x64) & 0x20) == 0)
		return false;
	
	switch(m_state)
	{
		case 0: 
			m_data[0] = inb(DATA_PORT);
			m_state++;
		break;
		
		case 1:
			m_data[1] = inb(DATA_PORT);
			m_state++;
		break;
		
		case 2: {
			m_state = 0;
			m_data[2] = inb(DATA_PORT);
			IO::HIDEvent event;
			
			bool newState = false;
			// Left button
			if((newState = (m_data[0] & 0x1)) != m_buttons[0])
			{
				m_buttons[0] = newState;
				event.type = (newState) ? IO::HID_KEY_DOWN : IO::HID_KEY_UP;
				event.data = 0;
				notifyObservers(event);
				return true;
			}
			else // Right button
			if((newState = (m_data[0] & 0x2)) != m_buttons[1])
			{
				m_buttons[1] = newState;
				event.type = (newState) ? IO::HID_KEY_DOWN : IO::HID_KEY_UP;
				event.data = 1;
				notifyObservers(event);
				return true;
			}
			else// Middle button
			if((newState = (m_data[0] & 0x4)) != m_buttons[2])
			{
				m_buttons[2] = newState;
				event.type = (newState) ? IO::HID_KEY_DOWN : IO::HID_KEY_UP;
				event.data = 2;
				notifyObservers(event);
				return true;
			}
			
			event.type = IO::HID_AXIS_MOVE;
			event.data = (m_data[0] & 0x10) ? ((unsigned int) m_data[1]) | 0xFFFFFF00 : m_data[1];
			event.data1 = (m_data[0] & 0x20) ? ((unsigned int) m_data[2]) | 0xFFFFFF00 : m_data[2];;
			notifyObservers(event);
	
		}
		break;
	}
	return true;
}

#include "AnsiTerminal.h"
#include <sstream>
#include <HIDDevice.h>

#include <signal.h>

bool AnsiTerminal::initialize()
{
	std::stringstream path;
	path << "/dayos/dev/tty" << m_id;
	
	int retval = vfs_create_device(path.str().c_str(), VFS_MODE_RW, VFS_CHARACTER_DEVICE);
	if (retval == SIGNAL_FAIL)
	{
		debug_printf("[ TTY ] Could not create device file!\n");
		return false;
	}
	
	memset(modifiers, 0, sizeof(modifiers));
	return true;
}

size_t AnsiTerminal::write(void* what, size_t size)
{
	m_framebuffer.puts((const char*) what, size);	
	return size;
}


bool AnsiTerminal::handle(message_t& msg)
{
	if(IO::CharDevice::handle(msg))
	{
		return true;
	}
	
	IO::HIDEvent* event = (IO::HIDEvent*) &msg.message;	
	if(event->type == IO::HID_KEY_DOWN)
	{
		switch(event->data)
		{
			case 0x2A: // Shift
			case 0x36:
				modifiers[0] = true;
			break;
			
			case 0x1D: // Ctrl
				modifiers[2] = true;
			break;
			
			case 0x38: // Alt
				modifiers[1] = true;
			break;
			
			default: {
				if(modifiers[0])
					event->data += 0x100;
				if(modifiers[1])
					event->data += 0x200;
				if(modifiers[2])
				{
					int character = m_keymap[event->data];

					// Generate signals!
					if(hasNextRequest())
					{
						switch(character)
						{
						case 'c':
							kill(getNextRequestPID(), SIGINT);
							m_framebuffer.puts("^C", 2);
							break;
							
						case 'd':
							m_framebuffer.putch(-1);
							m_framebuffer.puts("^D", 2);
							break;
						}
					}
					return true;
				}
				
				int character = m_keymap[event->data];
				if(character == '\b' && getBufferSize() > 0)
					m_framebuffer.putch(character);
				else if(character != '\b')
					m_framebuffer.putch(character);

				putch(character);
			}
		}
		return true;
	}
	else if(event->type == IO::HID_KEY_UP)
	{
		switch(event->data)
		{
			case 0x2A: // Shift
			case 0x36:
				modifiers[0] = false;
			break;
			
			case 0x1D: // Ctrl
				modifiers[2] = false;
			break;
			
			case 0x38: // Alt
				modifiers[1] = false;
			break;
		}
		
		return true;
	}
	return false;
}

#include "AnsiTerminal.h"
#include <sstream>

#include <termios.h>
#include <sys/ioctl.h>
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
	
	c_iflag = BRKINT;
	c_lflag = ECHO | ECHOE | ISIG | ICANON;
	
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
	
	switch(msg.signal)
	{
		case IO::HID_EVENT:
			return handleHIDEvent((IO::HIDEvent*) &msg.message);
		break;
		
		case VFS_SIGNAL_CONFIGURE:
		{
			struct vfs_config_request* rq = (struct vfs_config_request*) &msg.message;
			int value = atoi(rq->value);
			
			msg.signal = SIGNAL_FAIL;
			if(!strcmp(rq->key, "termios.c_iflag"))
			{
				c_iflag = value;
				msg.signal = SIGNAL_OK;
			}
			else if(!strcmp(rq->key, "termios.c_lflag"))
			{
				c_lflag = value;
				msg.signal = SIGNAL_OK;
			}
			
			send_message(&msg, msg.sender);
		}
		break;
	}
	
	return false;
}


bool AnsiTerminal::handleHIDEvent(IO::HIDEvent* event)
{
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
				if(character == '\b' && getBufferSize() > 0 && c_lflag & ECHOE)
					m_framebuffer.putch(character);
				else if(character != '\b' && c_lflag & ECHO)
					m_framebuffer.putch(character);
				
				putch(character, c_lflag & ICANON);
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dayos.h>
#include <sys/ioctl.h>
#include "GenericKeyboard.h"
#include "GenericMouse.h"

bool initkbd(GenericKeyboard& kbd)
{
	if(!kbd.initialize())
	{
		debug_printf("[ KBD ] Could not initialize device!\n");
		return false;
	}
	
	debug_printf("[ KBD ] Found keyboard '%s'\n", kbd.getName());

	// Register for receiving IRQs
	register_handler_process(0x21);
	register_service("keyboard");
	kbd.setLed(0, true);
	
	debug_printf("[ KBD ] Driver started.\n");

	return true;
}

bool initmouse(GenericMouse& mouse)
{
	if(!mouse.initialize())
	{
		debug_printf("[ MOUSE ] Could not initialize device!\n");
		return false;
	}
	
	register_handler_process(0x20 + 12);
	register_service("mouse");

	debug_printf("[ MOUSE ] Driver started.\n");
	return true;
}

int main()
{
	GenericKeyboard kbd;
	GenericMouse mouse;
	
	bool hasKbd = initkbd(kbd);
	bool hasMouse = initmouse(mouse);
	
	message_t msg;
	while(true)
	{
		while(receive_message(&msg, MESSAGE_ANY) != MESSAGE_RECEIVED) sleep(10);
		if(hasKbd && kbd.handle(msg)) continue;
		if(hasMouse) mouse.handle(msg);
	}

	return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dayos.h>
#include <sys/ioctl.h>
#include "RAMFilesystem.h"

int main()
{
	message_t msg;
	while(true)
	{
		while(receive_message(&msg, MESSAGE_ANY) != MESSAGE_RECEIVED) sleep(10);
	}

	return 0;
}

#include <stdio.h>
#include <string.h>
#include <dayos.h>
#include <driver.h>
#include <stdlib.h>
#include <errno.h>
#include "FloppyDrive.h"

/*void readSectorToBuffer(uint32_t lba, char* buffer, uint32_t* currentBlock)
{
	if (*currentBlock != lba)
	{
		*currentBlock = lba;
		ReadSectorLBA(0, lba);
		memcpy(buffer, (void*) 0x1000, 512);
	}
}*/

#define min(a, b) ((a < b) ? a : b)

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

int main(int argc, char** argv)
{
	// Check if FDC exists
	outb(0x70, 0x10);
	uint8_t drives = inb(0x71) >> 4;

	if (drives == 0)
	{
		return 0;
	}

	debug_printf("[ FDC ] Found %d floppy drives.\n", drives);
	register_handler_process(0x26);

	FloppyDrive drive(0);
	drive.initialize();

	//char sector[512];
	//drive.readBlock(0, sector);

	/*debug_printf("READ...\n");

	int i = 0;
	int c, j;
	for (c = 0; c < 4; ++c)
	{
		for (j = 0; j < 128; ++j)
			debug_printf("%x ", (unsigned int) sector[i + j]);
		i += 128;
		debug_printf("\n\n");
	}*/

	message_t msg;
	while (1)
	{
		while (receive_message(&msg, MESSAGE_ANY) != MESSAGE_RECEIVED)
			sleep(10);
		drive.handle(msg);
	}

	return 0;
}

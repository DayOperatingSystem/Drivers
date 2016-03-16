#include "FloppyDrive.h"
#include <cerrno>
#include <cstring>
#include <sstream>

#define SRA 0x3F0
#define SRB 0x3F1
#define DOR 0x3F2
#define TDR 0x3F3
#define MSR 0x3F4
#define DRSR 0x3F4
#define FIFO 0x3F5
#define DIR 0x3F7
#define CCR 0x3F7

#define DOR_INIT_BITMASK 0xC

#define CMD_VERSION 0x10
#define CMD_CONFIGURE 0x13
#define CMD_LOCK 0x94
#define CMD_UNLOCK 0x14
#define CMD_SPECIFY 0x3
#define CMD_CALIBRATE 0x7
#define CMD_SENSE 0x8
#define CMD_READ 0x46
#define CMD_SEEK 0xF

#define MSR_DATAREG 0x80
#define MSR_DIO_TO_CPU 0x40

#define DOR_RESET 4
#define DOR_DMA 8

#define ERR_TIMEOUT 1
#define NO_ERROR 0

#define ERR_NOT_SUPPORTED 2


void outb(unsigned short port, unsigned char value);
unsigned char inb(unsigned short port);

void FloppyDrive::WriteTransfer()
{
	outb(0x0A, 0x06); // Channel 2 Mask
	outb(0x0B, 0x5A); // DMA-Write
	// single transfer, address increment, autoinit, write, channel2
	// 01011010

	/*outb(0x81, 0x1000 >> 16);
	outb(0x04, (0x1000 & 0xffff) & 0xFF);
	outb(0x04, (0x1000 & 0xffff) >> 8);*/

	outb(0x0A, 0x02); // Channel 2 Unmask
}

void FloppyDrive::ReadTransfer()
{
	outb(0x0A, 0x06); // Channel 2 Mask
	outb(0x0B, 0x56); // DMA-Read

	/*outb(0x81, 0x1000 >> 16);
	outb(0x04, (0x1000 & 0xffff) & 0xFF);
	outb(0x04, (0x1000 & 0xffff) >> 8);*/

	outb(0x0A, 0x02); // Channel 2 Unmask
}

int FloppyDrive::WaitIRQ()
{
	message_t msg;
	msg.signal = 0;

	for (int i = 0; i < 50; i++)
	{
		if(receive_message(&msg, 0) == MESSAGE_RECEIVED)
			return NO_ERROR;

		sleep(100);
	}

	return ERR_TIMEOUT;
}

int FloppyDrive::SendCommand(uint8_t cmd)
{
	for (uint32_t timeout = 0; timeout < 100; timeout++)
	{
		if ((inb(MSR) & 0xC0) == 0x80)
		{
			outb(FIFO, cmd);
			inb(MSR);
			return NO_ERROR;
		}
		sleep(10);
	}
	return ERR_TIMEOUT;
}

uint8_t FloppyDrive::ReadData()
{
	for (uint32_t timeout = 0; timeout < 100; timeout++)
	{
		if ((inb(MSR) & 0xD0) == 0xD0)
		{
			errno = NO_ERROR;
			return inb(FIFO);
		}
		sleep(10);
	}
	errno = ERR_TIMEOUT;
	return 0;
}

void FloppyDrive::InitDMA()
{

	// ISA DMA stuff
	outb(0x0a, 0x06); // mask DMA channel 2 and 0 (assuming 0 is already masked)
	outb(0x0c, 0xFF); // reset the master flip-flop
	outb(0x04, 0);	// address to 0 (low byte)
	outb(0x04, 0x10); // address to 0x10 (high byte)
	outb(0x0c, 0xFF); // reset the master flip-flop (again!!!)
	outb(0x05, 0xFF); // count to 0x23ff (low byte)
	outb(0x05, 0x23); // count to 0x23ff (high byte),
	outb(0x81, 0); // external page register to 0 for total address of 00 10 00
	outb(0x0a, 0x02); // unmask DMA channel 2
}


void FloppyDrive::Select(int driveNumber)
{
	uint8_t dor = inb(DOR);
	outb(DOR, (dor & (~0x03)) | driveNumber);
}

void FloppyDrive::MotorOn(int driveNumber)
{
	uint8_t status = inb(0x3F2);
	outb(0x3F2, (status & 0x0F) | (0x10 << driveNumber));
	sleep(500);
}

void FloppyDrive::MotorOff(int driveNumber)
{
	uint8_t status = inb(0x3F2);
	outb(0x3F2, status & ~(0x10 << driveNumber));
}

void FloppyDrive::Sense(unsigned char* status, unsigned char* cylinder)
{
	SendCommand(CMD_SENSE);
	*status = ReadData();
	*cylinder = ReadData();
}

void FloppyDrive::Calibrate(int driveNumber)
{
	uint8_t st0, cyl0;

	Select(driveNumber);
	MotorOn(driveNumber);

	SendCommand(CMD_CALIBRATE);
	SendCommand(driveNumber);

	if(WaitIRQ() == ERR_TIMEOUT)
	{
		debug_printf("[ FDC ] Timeout while waiting for IRQ!\n");
	}

	//Sense(&st0, &cyl0);

	MotorOff(driveNumber);
}

int FloppyDrive::Seek(int driveNumber, uint8_t cylinder, uint8_t head)
{
	uint8_t st0, cyl0;

	int i = 0;
	for(i = 0; i < 5; i++)
	{
		SendCommand(CMD_SEEK);
		SendCommand((head << 2) | driveNumber);
		SendCommand(cylinder);

		WaitIRQ();

		Sense(&st0, &cyl0);

		if(cyl0 == cylinder)
			return NO_ERROR;

		sleep(15);
	}

	return ERR_TIMEOUT;
}

/// FIXME: Intelligente Motorverwaltung
int FloppyDrive::ReadSector(int driveNumber, uint8_t cylinder, uint8_t head, uint8_t sector)
{
	uint8_t cyl0, status;

	ReadTransfer();

	MotorOn(driveNumber);

	Seek(driveNumber, cylinder, head);

	for (int i = 0; i<5; i++)
	{
		SendCommand(CMD_READ);
		SendCommand((head<<2) | driveNumber);
		SendCommand(cylinder);
		SendCommand(head);
		SendCommand(sector);

		SendCommand(2);
		SendCommand(18);
		SendCommand(27);
		SendCommand(0xFF);

		WaitIRQ();

		uint8_t st0 = ReadData();  //st0
		ReadData();  //st1
		ReadData();  //st2
		ReadData();  //cylinder
		ReadData();  //head
		ReadData();  //sector
		ReadData();  //sectorsize

		Sense(&cyl0, &status);

		if((st0 & 0xC0) == 0)
		{
			MotorOff(driveNumber);
			return NO_ERROR;
		}
	}

	MotorOff(driveNumber);
	return ERR_TIMEOUT;
}

int FloppyDrive::ReadSectorLBA(int drive, uint32_t lba)
{
	uint8_t sector = (lba % 18) + 1;
	uint8_t cylinder = (lba / 18) / 2;
	uint8_t head = (lba / 18) % 2;

	return ReadSector(drive ,cylinder, head, sector);
}

int FloppyDrive::VersionCommand()
{
	uint8_t result;
	SendCommand(CMD_VERSION);
	result = ReadData();

	if(errno != NO_ERROR)
	{
		debug_printf("[ FDC ] Version command failed!\n");
		return errno;
	}
	else if(result != 0x90)
	{
		return ERR_NOT_SUPPORTED;
	}

	return NO_ERROR;
}

int FloppyDrive::ConfigureAndLock()
{
	// Configure
	int ret = SendCommand(CMD_CONFIGURE);

	ret = SendCommand(0);
	ret = SendCommand((1 << 6) | (0 << 5) | (0 << 4) | 7);
	ret = SendCommand(0);

	// FIXME: Lock
	/*ret = SendCommand(CMD_LOCK);

	if(ReadData() != (0x80 << 4))
	{
		printf("[ FLOPPY ] Warnung: Konnte das Laufwerk nicht sperren!\n");
	} */

	return ret;
}

void FloppyDrive::Reset()
{
	outb(DOR, 0);
	outb(DOR, 0x0C);

	WaitIRQ();

	for(int i = 0; i < 4; i++)
	{
		SendCommand(CMD_SENSE);
		ReadData();
		ReadData();
	}

	outb(CCR, 0);

	//SendCommand(CMD_SPECIFY);
	//outb(FIFO, steprate_headunload);
	//outb(FIFO, headload_ndma);
}


bool FloppyDrive::readBlock(size_t offset, void* data)
{
	int error = ReadSectorLBA(m_devno, offset);

	if(error != NO_ERROR)
		return false;

	memcpy(data, (void*) 0x1000, getBlocksize());
	return true;
}

bool FloppyDrive::writeBlock(size_t offset, void* data)
{
	return false;
}

bool FloppyDrive::handle(message_t& msg)
{
	if(IO::BlockDevice::handle(msg))
		return true;

	return false;
}

bool FloppyDrive::initialize()
{
	std::stringstream devstring;
	devstring << "/dayos/dev/fdc" << m_devno;
	int retval = vfs_create_device(devstring.str().c_str(), VFS_MODE_RW, VFS_BLOCK_DEVICE);
	if (retval == SIGNAL_FAIL)
	{
		debug_printf("[ FDC ] Could not create device file!\n");
		return false;
	}

	InitDMA();
	Reset();

	int result = VersionCommand();

	if(result == ERR_NOT_SUPPORTED)
	{
		debug_printf("[ FDC ] Your drive is not supported!\n");
		return false;
	}

	if(ConfigureAndLock() == ERR_TIMEOUT)
	{
		debug_printf("[ FDC ] Timeout while configuring drive!\n");
		return false;
	}

	Calibrate(m_devno);

	// Hard code for 1.44M floppies
	setBlockCount(2880);
	debug_printf("[ FDC ] Initialized drive %d\n", m_devno);
	return true;
}
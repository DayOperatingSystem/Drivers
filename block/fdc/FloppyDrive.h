#ifndef FLOPPYDRIVE_H
#define FLOPPYDRIVE_H

#include <BlockDevice.h>

class FloppyDrive : public IO::BlockDevice
{
	void Reset();
	int ConfigureAndLock();
	int VersionCommand();
	int ReadSectorLBA(int drive, uint32_t lba);
	int ReadSector(int driveNumber, uint8_t cylinder, uint8_t head, uint8_t sector);
	int Seek(int driveNumber, uint8_t cylinder, uint8_t head);
	void Calibrate(int driveNumber);
	void Sense(unsigned char* status, unsigned char* cylinder);
	void MotorOff(int driveNumber);
	void MotorOn(int driveNumber);
	void Select(int driveNumber);
	void InitDMA();
	uint8_t ReadData();
	int SendCommand(uint8_t cmd);
	int WaitIRQ();
	void ReadTransfer();
	void WriteTransfer();

	uint32_t m_devno;
public:

	FloppyDrive(uint32_t devno) : IO::BlockDevice(512), m_devno(devno) {}

	virtual bool initialize();
	virtual bool readBlock(size_t offset, void* data);
	virtual bool writeBlock(size_t offset, void* data);
	virtual bool handle(message_t& msg);
};

#endif // FLOPPYDRIVE_H

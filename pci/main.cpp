#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dayos.h>
#include <sys/ioctl.h>
#include <string>
#include <syscall.h>
#include <arch.h>

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC
 
#define PCI_VENDOR_ID   0x00
#define PCI_DEVICE_ID   0x02
#define PCI_COMMAND     0x04
#define PCI_STATUS      0x06
#define PCI_REVISION    0x08
#define PCI_CLASS       0x0B
#define PCI_SUBCLASS    0x0A
#define PCI_INTERFACE   0x09
#define PCI_HEADERTYPE  0x0E
#define PCI_BAR0        0x10
#define PCI_INTERRUPT   0x3C

struct PCIDevice
{
	uint32_t vendor, device;
	uint32_t subvendor, subclass;
	uint32_t devclass, mask;
};

#define MAX_CLASS 0x11
const char* pci_classes[] = {
	"Unknown",
	"Mass Storage Controller",
	"Network Controller",
	"Display Controller",
	"Multimedia Controller",
	"Memory Controller",
	"Bridge Device",
	"Simple Communication Controller",
	"Base System Peripheral",
	"Input Device",
	"Docking Station",
	"Processor",
	"Serial Bus Controller",
	"Wireless Controller",
	"Intelligent I/O Controller",
	"Satellite Communication Controller",
	"Encryption/Decryption Controller",
	"Data Acquisition and Signal Processing Controller"
};

void outl(unsigned short port, unsigned long value)
{
	syscall3(SYSCALL_WRITE_PORT, port, 4, value);
	//asm volatile("outl %1, %0" : : "dN"(port), "a"(value));
}

unsigned int inl(unsigned short port)
{
	//unsigned long ret;
	//asm volatile("inl %1, %0" : "=a"(ret) : "dN"(port));
	return syscall2(SYSCALL_READ_PORT, port, 4);
}

uint32_t readConfig(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg)
{
    uint32_t offset = reg % 0x04;
    outl(PCI_CONFIG_ADDR, 
        0x1 << 31
        | ((bus     & 0xFF) << 16) 
        | ((device  & 0x1F) << 11) 
        | ((func    & 0x07) <<  8) 
        | ((reg     & 0xFC)));

    return inl(PCI_CONFIG_DATA) >> (8 * offset);
}

bool checkDevice(uint32_t bus, uint32_t slot, PCIDevice& device)
{
	uint16_t vendor = readConfig(bus, slot, 0, PCI_VENDOR_ID);
	if(vendor == 0xFFFF)
		return false;

	device.vendor = vendor;
	device.device = readConfig(bus, slot, 0, PCI_DEVICE_ID);
	
	device.devclass = readConfig(bus, slot, 0, PCI_CLASS) & 0xff;
	device.subclass = readConfig(bus, slot, 0, PCI_SUBCLASS) & 0x00ff;
	
	debug_printf("[ PCI ] Found PCI device at %d:%d %x %x %s %x\n", bus, slot, vendor, device.device, pci_classes[device.devclass], device.subclass);

	return true;
}

void pciScan()
{
	PCIDevice device;
	for(int bus = 0; bus < 256; bus++)
	{
		for(int dev = 0; dev < 32; dev++)
		{
			if(checkDevice(bus, dev, device))
			{
			}
		}
	}
}

int main(int argc, char** argv)
{
	pciScan();
	message_t msg;
	while(true)
	{
		while(receive_message(&msg, MESSAGE_ANY) != MESSAGE_RECEIVED) sleep(10);
	}

	return 0;
}

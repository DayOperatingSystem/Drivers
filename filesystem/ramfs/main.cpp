#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dayos.h>
#include <sys/ioctl.h>
#include "RAMFilesystem.h"

#ifndef DEFAULT_MOUNTPOINT
#define DEFAULT_MOUNTPOINT "/drives/ramfs"
#endif

int main(int argc, char* argv[])
{
	RAMFilesystem rfs;
	message_t msg;

	const char* mountpoint = DEFAULT_MOUNTPOINT;
	// Mount myself
	if(argc > 1)
		mountpoint = argv[1];

	if(vfs_mount_ramdisk(mountpoint, VFS_MODE_RW) != SIGNAL_OK)
	{
		debug_printf("[ RAMFS ] Could not mount RAM disk at '%s'\n", mountpoint);
		return 1;
	}

	debug_printf("[ RAMFS ] Mounted at '%s'\n", mountpoint);
	while(true)
	{
		while(receive_message(&msg, MESSAGE_ANY) != MESSAGE_RECEIVED) sleep(10);
		rfs.handle(msg);
	}

	return 0;
}

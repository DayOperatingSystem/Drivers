#ifndef RAMFILESYSTEM_H
#define RAMFILESYSTEM_H

#include <FileSystem.h>

class RAMFilesystem : public IO::FileSystem
{
public:
	RAMFilesystem() : IO::FileSystem() {}
	bool initialize();
	bool handle(message_t& msg);
	const char* getName() { return "RAMFS"; }


};

#endif // RAMFILESYSTEM_H
